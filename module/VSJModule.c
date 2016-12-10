#include <linux/module.h>  /* Needed by all kernel modules */
#include <linux/kernel.h>  /* Needed for loglevels (KERN_WARNING, KERN_EMERG, KERN_INFO, etc.) */
#include <linux/init.h>    /* Needed for __init and __exit macros. */

#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function

#include <linux/rhashtable.h>
//

#include <linux/hash.h>
#include <linux/slab.h>
#include "VSJModule.h"          /* Needed for handling the DB with Key Value */

#define  DEVICE_NAME "key_value_DB_char"    ///< The device will appear at /dev/key_value_DB_char using this value
#define  CLASS_NAME  "key_value_DB"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("VSJ");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

struct hashed_object {
    int key;
    void * value;
    size_t size; //size of the value
    struct rhash_head node;
};

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer
static struct rhashtable *ht;
static struct rhashtable_params params = {
    .head_offset = offsetof(struct hashed_object, node),
    .key_offset = offsetof(struct hashed_object, key),
    .key_len = sizeof(int),
    .hashfn = jhash,
    .nulls_base = (1U << RHT_BASE_SHIFT),
};

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int    setupNewKVDB(void);
static int KVDB_add (int key, void *val, size_t size);
static int KVDB_remove (int *key);
static void KVDB_free_fn(void *ptr, void *arg);




/*
 struct rhashtable_params {
   size_t        nelem_hint;
   size_t        key_len;
   size_t        key_offset;
   size_t        head_offset;
   u32        hash_rnd;
   size_t        max_shift;
   rht_hashfn_t     hashfn;
   rht_obj_hashfn_t  obj_hashfn;
   bool       (*grow_decision)(const struct rhashtable *ht,
                  size_t new_size);
   bool       (*shrink_decision)(const struct rhashtable *ht,
                    size_t new_size);
   int        (*mutex_is_held)(void);
    };

*/



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
//   .setupNewKVDB = setupNewKVDB,
};

static int __init onload(void) {
    char *teststr;
    struct hashed_object *testobj;
    int ret, i ,j;

  printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");
  ret = setupNewKVDB();
  if(ret == 0){
      printk(KERN_INFO "EBBChar : kvdb returned \n");
  } else {
      printk("EBBChar : kvdb not pos\n");
      return ret;
   }

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "EBBChar failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "EBBChar: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "EBBChar: device class registered correctly\n");

   // Register the device driver
   ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
      class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ebbcharDevice);
   }
   printk(KERN_INFO "EBBChar: device class created correctly\n"); // Made it! device was initialized

   //testing the rhashtable
   for(i = 0; i < 10; ++i) {
       teststr = kmalloc((i + 2) * sizeof(char), GFP_KERNEL);
       for(j = 0; j < (i + 1); ++j){
           *(teststr + j) = 'a';
       }
       *(teststr + i + 1) = '\0';
       printk(KERN_INFO "VSJ\n");
       printk(KERN_INFO "VSJ: str %s\n", teststr);
       if(KVDB_add(i, teststr, (size_t) i+2)){
           printk(KERN_INFO "VSJ memory\n");
       }
       teststr = NULL;
   }
   for(i = 0; i < 10; ++i){
       testobj = KVDB_lookup(ht, &i, params);
       if(testobj == NULL){
           printk(KERN_INFO "VSJModule: lookup for key %d ret null\n", i);
       } else {
           printk(KERN_INFO "VSJModule: value in key %d is %s\n", i, (char*)testobj->value);
           KVDB_remove(&i);
       }
   }
   return 0;
}

static void __exit onunload(void) {
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   rhashtable_free_and_destroy(ht, &KVDB_free_fn, NULL);
   printk(KERN_INFO "EBBChar: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
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
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   } else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
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
   int nc;
   nc = copy_from_user(&op,buffer,1);
   if(nc){
       printk(KERN_INFO "VSJModule:in write copy 1 %d bytes not copied\n", nc);
   }
   nc = copy_from_user(&key,&buffer[1],4);
   if(nc){
       printk(KERN_INFO "VSJModule:in write copy 2 %d bytes not copied\n", nc);
   }
   if(op == 0) {
   		nc = copy_from_user(message,&buffer[5],len-5);
        if(nc){
            printk(KERN_INFO "VSJModule:in write copy 2 %d bytes not copied\n", nc);
        }
   }
   printk(KERN_INFO "EBBChar: Received %zu characters from the user, op:%d, key:%d, value:%s\n ", len , op, key, message);
   return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
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
    return ret;
        /*struct rhashtable_params *rhp;
        rhp =kmalloc(sizeof(struct rhashtable_params),GFP_KERNEL);
        rhp->nelem_hint = 1024;
        rhp->key_len =      4;
        rhp->key_offset = 0;
        rhp->head_offset =0;
        rhp->hashfn = NULL;
        rhp->obj_hashfn=NULL;
        rhash_head *inst=kmalloc(sizeof(struct rhash_head),GFP_KERNEL);
        struct rhashtable *hashTable =kmalloc(sizeof(struct rhashtable),GFP_KERNEL);
        struct hashed_object *objectet = kmalloc(sizeof(struct hashed_object),GFP_KERNEL);
        inst->key=10;
        //   inst->value=
        //rhp->hash_rnd  =  34;
        //rhp->max_shift  = 13555;
        //rhp->hashfn = arch_fast_hash();
        //rhp->mutex_is_held= false;
        //__rhashtable_insert_fast(   struct rhashtable *ht, const void *key, struct rhash_head *obj,
        //  const struct rhashtable_params params)
        int initint=   rhashtable_init(hashTable,rhp);
        rhashtable_insert_fast(hashTable,10,inst);
        printk(KERN_INFO "rhashtable iniited @ last w returnval : %d \n",initint);
        https://lwn.net/Articles/612100/ Demonstrates
        //rhashtable_insert(struct rhashtable *ht, struct rhash_head *node,gfp_t gfp_flags);
        // rhashtable_insert(hashTable,objectet->node,NULL);
    return ret;
        //  struct rhashtable_params {
        // size_t        nelem_hint;
        // size_t        key_len;
        // size_t        key_offset;
        // size_t        head_offset;
        // u32        hash_rnd;
        // size_t        max_shift;
        // rht_hashfn_t     hashfn;
        // rht_obj_hashfn_t  obj_hashfn;
        // bool       (*grow_decision)(const struct rhashtable *ht,
        //                size_t new_size);
        // bool       (*shrink_decision)(const struct rhashtable *ht,
        //                  size_t new_size);
        // int        (*mutex_is_held)(void);
        //  };*/
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
    ret = rhashtable_remove_fast(ht, &(obj->node), params);
    if(ret == 0){
        kfree(obj->value);
        kfree(obj);
    }
    return ret;
}

static void KVDB_free_fn(void *ptr, void *arg) {
    kfree(&((struct hashed_object *)ptr)->value);
    kfree(ptr);
}

module_init(onload);
module_exit(onunload);
