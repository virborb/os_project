#ifndef VSJMODULE_H
#define VSJMODULE_H

#define VSJ_GET     0
#define VSJ_SET     1
#define VSJ_DEL     2
#define VSJ_SAVE    3

#define  DEVICE_NAME "key_value_DB_char"    ///< The device will appear at /dev/key_value_DB_char using this value
#define  CLASS_NAME  "key_value_DB"        ///< The device class -- this is a character device driver

struct hashed_object {
    int key;
    void * value;
    size_t size; //size of the value
    struct rhash_head node;
};
struct hashed_key {
    pid_t pid;
    int key;
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
static void keyfree(void *ptr, void *arg);
static int lockAndRead(void);
static void resetIterations(void);

static inline struct hashed_object *KVDB_lookup(
        struct rhashtable *ht, const int *key,
	    const struct rhashtable_params params) {
    return (struct hashed_object*) rhashtable_lookup_fast(ht, key, params);
}

static inline struct hashed_key *getkey(struct rhashtable *keytable, const pid_t pid,
                        const struct rhashtable_params keytparams) {
    mutex_lock(&keytable->mutex);
    struct hashed_key *keystruct;
    keystruct = (struct hashed_key*) rhashtable_lookup_fast(keytable, &pid, keytparams);
    if(keystruct == NULL) {
        mutex_unlock(&keytable->mutex);
        return NULL;
    }
    rhashtable_remove_fast(keytable, &(keystruct->node), keytparams);
    mutex_unlock(&keytable->mutex);
    return keystruct;
}

static inline int addkey (int key, struct rhashtable *keytable, const pid_t pid,
                        const struct rhashtable_params keytparams){
    int ret;
    struct hashed_key *obj;
    obj = kmalloc(sizeof(struct hashed_key), GFP_KERNEL);
    if(unlikely(obj == NULL)){
        return -ENOMEM;
    }
    obj->key = key;
    obj->pid = pid;
    mutex_lock(&keytable->mutex);
    ret = rhashtable_insert_fast(keytable, &(obj->node), keytparams);
    if(ret != 0) {
        kfree(obj);
    }
    mutex_unlock(&keytable->mutex);
    return ret;
}

#endif
