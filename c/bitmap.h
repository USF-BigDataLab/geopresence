#ifndef BITMAP_H
#define BITMAP_H
#include "./roaring.h"

void init_bitmap(roaring_bitmap_t* bitmap);

void free_bitmap(roaring_bitmap_t* bitmap);

void add(roaring_bitmap_t* bitmap, uint32_t index);

// int get_cardinality();
// int to_array();
// static roaring_bitmap_t* from_bytes();
// long* byte_to_word();

#endif
