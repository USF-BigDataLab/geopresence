#include "uthash-master/src/uthash.h"

// This struct is used for storing data in the hashmap
// With this library, keep in mind that the pointer to
// the struct itself is the value for the hashmap.
struct bitmap_hm_data {
    int bitmap_cell; // This is the key for the bitmap; it's using the particular grid cell of the bitmap as they key.
    int cardinality; // Cardinality of the HyperLogLog
    UT_hash_handle hh; // makes this struct hashable 
};

void add_cell(struct bitmap_hm_data *bm_hm_d);