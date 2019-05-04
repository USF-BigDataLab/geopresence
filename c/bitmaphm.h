#ifndef BITMAPHM_H_INCLUDED
#define BITMAPHM_H_INCLUDED
#include "uthash-master/src/uthash.h"
#include "common.h"

// This struct is used for storing data in the hashmap
// With this library, keep in mind that the pointer to
// the struct itself is the value for the hashmap.
struct bitmap_hm_data {
    char* bitmap_cell; // This is the key for the bitmap; it's using the first two chars from the given hash
    struct rbitmap* bmap;
    UT_hash_handle hh; // makes this struct hashable 
};

void add_cell(char* cell, struct rbitmap* bm);
struct bitmap_hm_data *find_cell(char* cell);
void delete_cell(char* cell);
unsigned int get_hm_length();
void print_cells();
#endif