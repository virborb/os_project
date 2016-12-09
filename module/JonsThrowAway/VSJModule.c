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
//#include "kvdb.h"          /* Needed for handling the DB with Key Value */

#define  DEVICE_NAME "key_value_DB_char"    ///< The device will appear at /dev/key_value_DB_char using this value
#define  CLASS_NAME  "key_value_DB"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("VSJ");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static bool    setupNewKVDB(void);





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
  printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");
  if(setupNewKVDB())
      printk(KERN_INFO "EBBChar : kvdb returned \n");  
   else {
      printk("EBBChar : kvdb not pos \n");
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
   return 0;
}

static void __exit onunload(void) {
   device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
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
   copy_from_user(&op,buffer,1);
   copy_from_user(&key,&buffer[1],4);
   if(op = 0) {
   		copy_from_user(message,&buffer[5],len-5);
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


    struct hashed_object {
   int key;
   struct rhash_head node;
   /* Stuff of actual interest */
    } hashed_object;

static bool setupNewKVDB(void) {
   /** ACCORDING TO PREFACE: 
      https://lwn.net/Articles/611628/
   */
   printk(KERN_INFO "setupNewKVDB in progress @ last\n");
   struct rhashtable_params *rhp;
   rhp =kmalloc(sizeof(struct rhashtable_params),GFP_KERNEL);
   rhp->nelem_hint = 1024;
   rhp->key_len =      4;
   rhp->key_offset = 0;
   rhp->head_offset =0;
   rhp->obj_hashfn=NULL;
   struct rhash_head *inst=kmalloc(sizeof(struct rhash_head),GFP_KERNEL);
   struct rhashtable *hashTable =kmalloc(sizeof(struct rhashtable),GFP_KERNEL);
   struct hashed_object *objectet = kmalloc(sizeof(struct hashed_object),GFP_KERNEL);
    //inst->key=10;
    //   inst->value=
    //rhp->hash_rnd  =  34;
    //rhp->max_shift  = 13555;
    //rhp->hashfn = arch_fast_hash();
    //rhp->mutex_is_held= false;
    //__rhashtable_insert_fast{   struct rhashtable *ht, const void *key, struct rhash_head *obj,
    //  const struct rhashtable_params params)
   int initint=   rhashtable_init(hashTable,rhp);
    //const void *key= kmalloc(sizeof(int));
  // int inserted = rhashtable_insert_fast(hashTable,inst,*rhp);
    rhashtable_insert(hashTable,inst,GFP_KERNEL);
   //printk(KERN_INFO "rhashtable iniited @ last w returnval : %d, Insert returned = &d \n",initint,inserted);
    /*  */
    //rhashtable_insert(struct rhashtable *ht, struct rhash_head *node,gfp_t gfp_flags);
    // rhashtable_insert(hashTable,objectet->node,NULL);
    return true;
}

module_init(onload);
module_exit(onunload);