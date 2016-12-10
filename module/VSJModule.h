#ifndef VSJMODULE_H
#define VSJMODULE_H

static inline struct hashed_object *KVDB_lookup(
        struct rhashtable *ht, const int *key,
	    const struct rhashtable_params params) {
    return (struct hashed_object*) rhashtable_lookup_fast(ht, key, params);
}

#endif
