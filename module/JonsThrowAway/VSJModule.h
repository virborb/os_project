#ifndef VSJMODULE_H
#define VSJMODULE_H

#define VSJ_GET 0
#define VSJ_SET 1
#define VSJ_DEL 2

#define  DEVICE_NAME "key_value_DB_char"    ///< The device will appear at /dev/key_value_DB_char using this value
#define  CLASS_NAME  "key_value_DB"        ///< The device class -- this is a character device driver

struct hashed_object {
    int key;
    void * value;
    size_t size; //size of the value
    struct rhash_head node;
};

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int    setupNewKVDB(void);
static int KVDB_add (int key, void *val, size_t size);
static int KVDB_remove (int *key);
static void KVDB_free_fn(void *ptr, void *arg);
static int get_lock(void);
static void release_lock(void);

static inline struct hashed_object *KVDB_lookup(
        struct rhashtable *ht, const int *key,
	    const struct rhashtable_params params) {
    return (struct hashed_object*) rhashtable_lookup_fast(ht, key, params);
}

#endif
