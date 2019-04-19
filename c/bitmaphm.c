#include "bitmaphm.h"

/* Global hashmap with our data must be set to null in the beginning; variable name means global bitmap hashmap data */
struct bitmap_hm_data *g_bm_hm_data = NULL; 

void add_cell(struct bitmap_hm_data *bm_hm_d) {
    HASH_ADD_INT(g_bm_hm_data, bitmap_cell, bm_hm_d);
}