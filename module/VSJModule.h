#ifndef VSJMODULE_H
#define VSJMODULE_H

#define VSJ_GET 0
#define VSJ_SET 1
#define VSJ_DEL 2

static inline struct hashed_object *KVDB_lookup(
        struct rhashtable *ht, const int *key,
	    const struct rhashtable_params params) {
    return (struct hashed_object*) rhashtable_lookup_fast(ht, key, params);
}

#endif
