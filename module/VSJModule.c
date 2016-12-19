#include <linux/module.h>  /* Needed by all kernel modules */
#include <linux/kernel.h>  /* Needed for loglevels (KERN_WARNING, KERN_EMERG, KERN_INFO, etc.) */
#include <linux/init.h>    /* Needed for __init and __exit macros. */
#include <linux/sched.h>
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/rwsem.h>

#include <linux/rhashtable.h>

#include <linux/hash.h>
#include <linux/slab.h>
#include "VSJModule.h"          /* Needed for handling the DB with Key Value */

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Victor Lundgren, Simon Vasterbo, Jon Leijon");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A Key-Value DB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
//static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
//static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  charClass  = NULL; ///< The device-driver class struct pointer
static struct device* charDevice = NULL; ///< The device-driver device struct pointer
//static int    getkey; //temporär lösnin
static struct rhashtable *ht, *keytable;
static struct rw_semaphore sem;
static struct rhashtable_params params = {
    .head_offset = offsetof(struct hashed_object, node),
    .key_offset = offsetof(struct hashed_object, key),
    .key_len = sizeof(int),
    .hashfn = jhash,
    .nulls_base = (1U << RHT_BASE_SHIFT),
};
static struct rhashtable_params ktparams = {
    .head_offset = offsetof(struct hashed_key, node),
    .key_offset = offsetof(struct hashed_key, pid),
    .key_len = sizeof(pid_t),
    .hashfn = jhash,
    .nulls_base = (1U << RHT_BASE_SHIFT),
};

static unsigned long max_val_size = 0;
static char *file_location = NULL;

module_param(max_val_size, ulong, 0644);
MODULE_PARM_DESC(max_val_size, "Maximum size of the value");
module_param(file_location, charp, 0644);
MODULE_PARM_DESC(file_location, "Where the saved key value store will be read from.");

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init onload(void) {
    int ret;

    printk(KERN_INFO "VSJModule: Initializing the VSJModule LKM\n");
    ret = setupNewKVDB();
    if(ret == 0){
        printk(KERN_INFO "VSJModule : kvdb returned \n");
    } else {
        printk("VSJModule : kvdb not pos\n");
        return ret;
    }

    if(max_val_size > SIZE_MAX) {
     max_val_size = SIZE_MAX;
    }

    printk(KERN_INFO "VSJ: max_val_size: %lu\n", max_val_size);
    if(file_location != NULL){
        printk(KERN_INFO "VSJ: file_location: %s\n", file_location);
    }


    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "VSJModule failed to register a major number\n");
        kfree(ht);
        kfree(keytable);
        return majorNumber;
    }
    printk(KERN_INFO "VSJModule: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(charClass)){                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        kfree(ht);
        kfree(keytable);
        return PTR_ERR(charClass);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "VSJModule: device class registered correctly\n");

   // Register the device driver
   charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(charDevice)){               // Clean up if there is an error
      class_destroy(charClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      kfree(ht);
      kfree(keytable);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(charDevice);
   }
   init_rwsem(&sem);
   printk(KERN_INFO "VSJModule: device class created correctly\n"); // Made it! device was initialized

    return 0;
}

static void __exit onunload(void) {
   device_destroy(charClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(charClass);                          // unregister the device class
   class_destroy(charClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   rhashtable_free_and_destroy(ht, &KVDB_free_fn, NULL);
   rhashtable_free_and_destroy(keytable, &keyfree, NULL);
   kfree(keytable);
   kfree(ht);
   printk(KERN_INFO "VSJModule: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   nonseekable_open(inodep, filep);
   printk(KERN_INFO "VSJModule: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    int err, cplen, key;
    struct hashed_object *obj;
    struct hashed_key *keyobj;
    pid_t pid;

   pid = task_pid_nr(current);
   keyobj = getkey(keytable, pid, ktparams);
   if(keyobj == NULL){
       return -EBADE;
   }
   key = keyobj->key;
   kfree(keyobj);
   down_read(&sem);
   obj = KVDB_lookup(ht, &key, params);
   if(obj == NULL) {
       up_read(&sem);
       return -ENOKEY;
   }
   cplen = len > obj->size ? obj->size : len;
   err = copy_to_user(buffer, obj->value, cplen);
   if(err) {
       up_read(&sem);
       return -EFAULT;
   }
   up_read(&sem);
   return cplen;
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   char op;
   int key = 0;
   int err, pid;
   void* val;
   int remlen = len;

   if(len < 5){
       return -EBADRQC;
   }
   err = copy_from_user(&op,buffer,1);
   if(err){
       printk(KERN_INFO "VSJModule:in write copy 1 %d bytes not copied\n", err);
       return -EFAULT;
   }
   printk(KERN_INFO"VSJModule req code: %c\n", op);
   remlen -= 1;
   err = copy_from_user(&key,&buffer[1],sizeof(int));
   if(err){
       printk(KERN_INFO "VSJModule:in write copy 2 %d bytes not copied\n", err);
       return -EFAULT;
   }
   remlen -= sizeof(int);
   switch (op) {
   case VSJ_GET:
        pid = task_pid_nr(current);
        err = addkey(key, keytable, pid, ktparams);
        if(err){
            return err;
        }
        return 5;
   case VSJ_SET:
        if(remlen < 1){
            return -ENODATA; //Ändra till lämpligare
        }
        if(max_val_size && remlen > max_val_size){
          return -EFBIG;
        }
        val = kmalloc(remlen, GFP_KERNEL);
        if(unlikely(val == NULL)){
            return -ENOMEM;
        }
        err = copy_from_user(val, &buffer[5], remlen);
        if(err){
            printk(KERN_INFO "VSJModule:in write SET %d bytes not copied\n", err);
            kfree(val);
            return -EFAULT;
        }
        err = KVDB_add(key, val, remlen);
        if(err){
            kfree(val);
            return err;
        }
        return len;
    case VSJ_DEL:
        return KVDB_remove(&key);
   }
   return -EBADRQC;
   /*printk(KERN_INFO "VSJModule: Received %zu characters from the user, op:%d, key:%d, value:%s\n ", len , op, key, message);*/
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "VSJModule: Device successfully closed\n");
   return 0;
}

static int setupNewKVDB(void) {
    /** ACCORDING TO PREFACE:
    https://lwn.net/Articles/611628/
    */
    int ret;
    printk(KERN_INFO "setupNewKVDB in progress @ last\n");
    ht = kmalloc(sizeof(struct rhashtable), GFP_KERNEL);
    if(ht == NULL){
        return -ENOMEM;
    }
    ret = rhashtable_init(ht, &params);
    if(ret != 0){
        kfree(ht);
    }

    keytable = kmalloc(sizeof(struct rhashtable), GFP_KERNEL);
    if(keytable == NULL){
        kfree(ht);
        return -ENOMEM;
    }
    ret = rhashtable_init(keytable, &ktparams);
    if(ret != 0){
        kfree(keytable);
        kfree(ht);
    }
    return ret;
}

static int KVDB_add (int key, void *val, size_t size){
    int ret;
    struct hashed_object *obj;
    obj = kmalloc(sizeof(struct hashed_object), GFP_KERNEL);
    if(unlikely(obj == NULL)){
        return -ENOMEM;
    }

    obj->key = key;
    obj->value = val;
    obj->size = size;
    ret = rhashtable_insert_fast(ht, &(obj->node), params);
    if(ret != 0){
        kfree(obj);
    }
    return ret;
}

static int KVDB_remove (int *key){
    struct hashed_object *obj;
    int ret;
    obj = KVDB_lookup(ht, key, params);
    if(obj == NULL){
        return -ENOENT;
    }
    down_write(&sem);
    ret = rhashtable_remove_fast(ht, &(obj->node), params);
    if(ret == 0){
        kfree(obj->value);
        kfree(obj);
    }
    up_write(&sem);
    return ret;
}

static void KVDB_free_fn(void *ptr, void *arg) {
    kfree(&((struct hashed_object *)ptr)->value);
    kfree(ptr);
}

static void keyfree(void *ptr, void *arg){
    kfree(ptr);
}

module_init(onload);
module_exit(onunload);
