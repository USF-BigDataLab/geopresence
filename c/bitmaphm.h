#include "uthash-master/src/uthash.h"

// This struct is used for storing data in the hashmap
// With this library, keep in mind that the pointer to
// the struct itself is the value for the hashmap.
struct bitmap_hm_data {
    char* bitmap_cell; // This is the key for the bitmap; it's using the first two chars from the given hash
    int cardinality; // Cardinality of the HyperLogLog
    UT_hash_handle hh; // makes this struct hashable 
};

void add_cell(char* cell, int card);
struct bitmap_hm_data *find_cell(char* cell);
void delete_cell(char* cell);
unsigned int get_hm_length();
void print_cells();