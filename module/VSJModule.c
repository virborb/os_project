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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Victor Lundgren, Simon Vasterbo, Jon Leijon");
MODULE_DESCRIPTION("A Key-Value DB");
MODULE_VERSION("0.1");

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static struct class*  charClass  = NULL; ///< The device-driver class struct pointer
static struct device* charDevice = NULL; ///< The device-driver device struct pointer
static struct rhashtable *ht, *keytable;

static struct rhashtable_iter *iter;       // to iterate ht
static struct hashed_object **saver;    // to list ht
static struct mutex ht_mutex;
static struct mutex kt_mutex;

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

static pid_t iteratorPID;
static int iteratorKey,iteratorLength;
static int getSize = 1;

module_param(max_val_size, ulong, 0644);
MODULE_PARM_DESC(max_val_size, "Maximum size of the value");

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
        .owner = THIS_MODULE,
        .open = dev_open,
        .read = dev_read,
        .write = dev_write,
        .release = dev_release,
};
/** @brief Creates a character device and rhashtables. Set
 *  max value size.
 */
static int __init onload(void) {
        int ret;
        printk(KERN_INFO "VSJModule: Initializing the VSJModule LKM\n");
        ret = setupNewKVDB();
        if(ret != 0) {
                printk(KERN_ERR "VSJModule : Error setting upp the hashtable\n");
                return ret;
        }
        if(max_val_size > SIZE_MAX) {
                max_val_size = SIZE_MAX;
        }
        majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
        if (majorNumber<0){
                printk(KERN_ERR "VSJModule: failed to register a major number\n");
                kfree(ht);
                kfree(keytable);
                return majorNumber;
        }
        charClass = class_create(THIS_MODULE, CLASS_NAME);
        if (IS_ERR(charClass)){
                unregister_chrdev(majorNumber, DEVICE_NAME);
                printk(KERN_ERR "VSJModule: Failed to register device class\n");
                kfree(ht);
                kfree(keytable);
                return PTR_ERR(charClass);
        }
        charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL,
                                   DEVICE_NAME);
        if (IS_ERR(charDevice)){
                class_destroy(charClass);
                unregister_chrdev(majorNumber, DEVICE_NAME);
                kfree(ht);
                kfree(keytable);
                printk(KERN_ERR "VSJModule: Failed to create the device\n");
                return PTR_ERR(charDevice);
        }
        mutex_init(&ht_mutex);
        mutex_init(&kt_mutex);
        printk(KERN_INFO "VSJModule: device class created correctly\n");
        return 0;
}

/** @brief Destroys character device and rhashtables.
 */
static void __exit onunload(void) {
        device_destroy(charClass, MKDEV(majorNumber, 0));
        class_unregister(charClass);
        class_destroy(charClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        rhashtable_free_and_destroy(ht, &KVDB_free_fn, NULL);
        rhashtable_free_and_destroy(keytable, &KVDB_keyfree, NULL);
        kfree(keytable);
        kfree(ht);
        if(iteratorKey == -1) {
                kfree(saver);
        }
        printk(KERN_INFO "VSJModule: unloaded.\n");

}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
        nonseekable_open(inodep, filep);
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
static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                        loff_t *offset){
        int err, cplen, key;
        struct hashed_object *obj;
        struct hashed_key *keyobj;
        pid_t pid;
        size_t isize, ssize;

        isize = sizeof(int);
        ssize = sizeof(size_t);
        pid = task_pid_nr(current);
        if(pid==iteratorPID) {
                if(iteratorKey==iteratorLength) {
                        resetIterations();
                        return 0;
                }
                if(getSize) {
                        cplen = len > (isize + ssize) ? (isize + ssize) : len;
                        err = copy_to_user(buffer+isize,
                                           &(saver[iteratorKey]->size), ssize);
                        err += copy_to_user(buffer, &(saver[iteratorKey]->key),
                                            isize);
                        if(err) {
                                return -EFAULT;
                        }
                        getSize = 0;
                } else {
                        cplen = len > saver[iteratorKey]->size ? saver[iteratorKey]->size : len;
                        err = copy_to_user(buffer, saver[iteratorKey]->value, cplen);
                        iteratorKey++;
                        getSize = 1;
                        if(iteratorKey > iteratorLength){
                                resetIterations();
                                return -ENODATA;
                        }
                        if(err) {
                                return -EFAULT;
                        }
                }
                return cplen;
        }

        keyobj = getkey(keytable, pid, ktparams, &kt_mutex);
        if(keyobj == NULL) {
                return -EBADE;
        }
        key = keyobj->key;
        kfree(keyobj);
        rcu_read_lock();
        obj = KVDB_lookup(ht, &key, params);
        if(obj == NULL) {
                rcu_read_unlock();
                return -ENOKEY;
        }
        cplen = len > obj->size ? obj->size : len;
        err = copy_to_user(buffer, obj->value, cplen);
        rcu_read_unlock();
        if(err) {
                return -EFAULT;
        }
        return cplen;
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user.
 *  Sets, gets or removes data from the hashtable.
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
        struct pid *saverpid = NULL;
        struct task_struct *savertask = NULL;

        if(len < 1){
                return -EBADRQC;
        }
        err = copy_from_user(&op,buffer,1);
        if(err){
                return -EFAULT;
        }
        remlen -= 1;
        err = copy_from_user(&key,&buffer[1],sizeof(int));
        if(err){
                return -EFAULT;
        }
        remlen -= sizeof(int);
        /**
        THIS is to block all SETS, DEL & SAVE
        during an active iteration.
        Checks that a task with that pid still exists, (it may be a zombie or
        a new process who got the pid after the saving process died)
        */
        if((iteratorPID>0) && op != VSJ_GET){
                err = -EFAULT;
                saverpid = find_get_pid(iteratorPID);
                rcu_read_lock();
                if(saverpid !=NULL) {
                        savertask = pid_task(saverpid, PIDTYPE_PID);
                        put_pid(saverpid);
                }
                if(savertask == NULL){
                        err = 0;
                }
                rcu_read_unlock();
                if(err){
                        return err;
                }
        }
        switch (op) {
        case VSJ_GET:
                pid = task_pid_nr(current);
                err = addkey(key, keytable, pid, ktparams, &kt_mutex);
                if(err){
                        return err;
                }
                return 5;
        case VSJ_SET:
                if(remlen < 1){
                        return -ENODATA;
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
        case VSJ_SAVE:
                lockAndRead();
                return 0;
        }
        return -EBADRQC;
}


static void resetIterations(void){
        mutex_lock(&ht_mutex);
        iteratorPID=-1;
        iteratorKey=-1;
        getSize = 1;
        if(saver != NULL){
                kfree(saver);
                saver = NULL;
        }
        mutex_unlock(&ht_mutex);
 }


static int lockAndRead(void) {
        int i;
        struct hashed_object *p;
        mutex_lock(&ht_mutex);
        if(iteratorPID>0){
                mutex_unlock(&ht_mutex);
                return -EINPROGRESS;
        }
        i=0;
        iteratorKey=0;
        iteratorPID=task_pid_nr(current);
        iter=kmalloc(sizeof(struct rhashtable_iter),GFP_KERNEL);
        rhashtable_walk_init(ht, iter);
        if(iter==NULL){
                printk(KERN_INFO "iter == Null");
                iteratorPID=-1;
                iteratorKey=-1;
                mutex_unlock(&ht_mutex);
                return -ENOMEM;
        }
        rhashtable_walk_start(iter);
        p = rhashtable_walk_next(iter);
        iteratorLength = atomic_read(&ht->nelems);
        saver=kmalloc(sizeof(struct hashed_object*)*iteratorLength, GFP_KERNEL);
        while(p!=NULL) {
                saver[i]=p;
                p=rhashtable_walk_next(iter);
                i++;
        }
        rhashtable_walk_stop(iter);
        rhashtable_walk_exit(iter);
        kfree(iter);
        iter = NULL;
        mutex_unlock(&ht_mutex);
        return 0;
}


/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
        pid_t pid = task_pid_nr(current);
        struct hashed_key *key;
        if(iteratorPID == task_pid_nr(current)){
                resetIterations();
        }
        key = getkey(keytable, pid, ktparams, &kt_mutex);
        if(key != NULL){
                kfree(key);
        }
        return 0;
}

/** @brief Sets up the rhastable for the key-value database and
 *  a rhashtable for the get requests.
 */
static int setupNewKVDB(void) {
        int ret;
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

/** @brief Added a key-value object to the rhashtable.
 *  @param key The key for the value
 *  @param val The value
 *  @param size The size of the value
 */
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
        ret = rhashtable_lookup_insert_fast(ht, &(obj->node), params);
        if(ret != 0){
                kfree(obj);
        }
        return ret;
}

/** @brief Remove a key-value object with the given key
 *  from the rhashtable
 *  @param key The key for the value to remove
 */
static int KVDB_remove (int *key){
        struct hashed_object *obj;
        int ret;
        int j;
        j=1;
        mutex_lock(&ht_mutex);
        printk("VSJ: %d", j);
        if(iteratorPID>0){
                mutex_unlock(&ht_mutex);
                return -EBUSY;
        }
                printk("VSJ: %d", j);
        obj = rhashtable_lookup_fast(ht, key, params);
                printk("VSJ: %d", j);
        if(obj == NULL){
                mutex_unlock(&ht_mutex);
                return -ENOENT;
        }
                printk("VSJ: %d", j);
        ret = rhashtable_remove_fast(ht, &(obj->node), params);
                printk("VSJ: %d", j);
        if(ret == 0){
                synchronize_rcu();
                kfree(obj->value);
                kfree(obj);
        }
                printk("VSJ: %d", j);
        mutex_unlock(&ht_mutex);
        return ret;
}

/** @brief Free function for the key-value rhashtable
 */
static void KVDB_free_fn(void *ptr, void *arg) {
        kfree(((struct hashed_object *)ptr)->value);
        kfree(ptr);
}

/** @brief Free function for get request rhashtable
 */
static void KVDB_keyfree(void *ptr, void *arg){
        kfree(ptr);
}

module_init(onload);
module_exit(onunload);
