 //#include <kernel/std>
#ifndef KVDB_H
#define KVDB_H
#include <linux/rhashtable.h>
#include <linux/hash.h>
#include <linux/slab.h>
 struct kvdbHolder 
 {
 	int noOfObjects;


 };

 struct KVDB_OBJECT 
 {
 	int key;
 	struct rhash_head node;
 	void *value;
 };

  /*  struct rhashtable_params {
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
    };
*/
/**
obj-m +=  VSJModule.o kvdb.o
VSJModule-y := VSJModule.o kvdb.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) KBUILD_EXTRA_SYMBOLS=$(PWD)/Module.symvers  modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
*/
bool setupNewKVDB(void);

#endif


