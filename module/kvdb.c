#include "kvdb.h"

static bool setupNewKVDB(void) {
	/** ACCORDING TO PREFACE: 
		https://lwn.net/Articles/611628/
	*/
	printk(KERN_INFO "setupNewKVDB in progress in different Doc!\n");
	/*rhashtable_params *rhp =kmalloc(sizeof(struct rhashtable_params),GFP_KERNEL);

	rhp->nelem_hint = 1024;
	rhp->key_len = 	  4;
	rhp->key_offset = 0;
	rhp->head_offset =0;*/
	//rhp->hash_rnd  =  34;
	//rhp->max_shift  = 13555;
	//rhp->hashfn = arch_fast_hash();
	//rhp->mutex_is_held= false;

	return true;
	/* struct rhashtable_params {
	size_t		  nelem_hint;
	size_t		  key_len;
	size_t		  key_offset;
	size_t		  head_offset;
	u32		  hash_rnd;
	size_t		  max_shift;
	rht_hashfn_t	  hashfn;
	rht_obj_hashfn_t  obj_hashfn;
	bool		  (*grow_decision)(const struct rhashtable *ht,
					   size_t new_size);
	bool		  (*shrink_decision)(const struct rhashtable *ht,
					     size_t new_size);
	int		  (*mutex_is_held)(void);
    };*/

}