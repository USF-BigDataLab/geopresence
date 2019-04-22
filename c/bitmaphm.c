#include "bitmaphm.h"

/* Global hashmap with our data must be set to null in the beginning; variable name means global bitmap hashmap data */
struct bitmap_hm_data *g_bm_hm_data = NULL; 

/* 
    Function: void add_cell(struct bitmap_hm_data *bm_hm_d)
    Input: int bm_cell - the bitmap cell to use as the key
           int card - the cardinality of the cell (value in hm)
    Output: None
    Purpose: This function is for adding more values to the hashmap
    We will pass in the data we want to put in the struct so that
    the function can do the memory allocation for us. 
*/
void add_cell(int bm_cell, int card) {
    struct bitmap_hm_data *hm_entry; 
    hm_entry = malloc(sizeof(struct bitmap_hm_data));
    hm_entry->bitmap_cell = bm_cell;
    hm_entry->cardinality = card; 
    /* Keep in mind that bitmap_cell is the name of the key field to use */
    HASH_ADD_INT(g_bm_hm_data, bitmap_cell, hm_entry); 
}

/* 
    Function: struct bitmap_hm_data *find_cell_data(int cell) 
    Input: int cell - the cell (key value) that we want to find the cardinality for;
    this is our way of finding a specific cell's cardinality in the hashmap. 
*/
struct bitmap_hm_data *find_cell_data(int cell) {
    struct bitmap_hm_data *found_entry;
    HASH_FIND_INT(g_bm_hm_data, &cell, found_entry);
    return found_entry;
}