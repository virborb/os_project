#include <linux/slab.h>
#include <linux/hash.h>
#include <linux/rhashtable.h>
#include "VSJmod_kvdb.h"

struct hashed_object {
    int key;
    void * value;
    size_t size; //size of the value
    struct rhash_head node;
};

static struct rhashtable ht;

static int setupNewKVDB(void) {
    /** ACCORDING TO PREFACE:
      https://lwn.net/Articles/611628/
    */
    struct rhashtable_params params;
    int ret;
    printk(KERN_INFO "setupNewKVDB in progress @ last\n");

    params = {
        .head_offset = offsetof(struct hashed_object, node);
        .key_offset = offsetof(struct hashed_object, value);
        .key_len = sizeof(int);
        .hashfn = jhash;
        .nulls_base = (1U << RHT_BASE_SHIFT);
    }

    ret = rhashtable_params(&ht, &params);
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
    /*https://lwn.net/Articles/612100/ Demonstrates*/
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
