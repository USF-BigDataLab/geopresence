#include "bitmaphm.h"
#include <stdio.h>

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
    Function: struct bitmap_hm_data *find_cell(int cell) 
    Input: int cell - the cell (key value) that we want to find the cardinality for;
    this is our way of finding a specific cell's cardinality in the hashmap. 
    Output: struct bitmap_hm-data *found_entry - returns the found entry in the hashmap
    Purpose: This function is for finding the struct data for the given cell
*/
struct bitmap_hm_data *find_cell(int cell) {
    struct bitmap_hm_data *found_entry;
    HASH_FIND_INT(g_bm_hm_data, &cell, found_entry);
    return found_entry;
}

/* 
    Function: void delete_cell(int cell)
    Input: int cell - the cell (key value) of the struct we want to delete from the hashmap
    Output: None
    Purpose: This function is for deleting a struct from the hashmap
*/
void delete_cell(int cell) {
    struct bitmap_hm_data *entry = find_cell(cell);
    HASH_DEL(g_bm_hm_data, entry);
    printf("The data at cell %d has been deleted and will be freed", cell);
    free(entry); // might need to be careful about where we free this data
}

/* 
    Function: unsigned int get_hm_length()
    Input: None
    Output: unsigned int - length of the hashmap
    Purpose: This function is for finding how many elements are in the hashmap
*/
unsigned int get_hm_length(){
    unsigned int num_cells;
    num_cells = HASH_COUNT(g_bm_hm_data);
    return num_cells;
}

/* 
    Function: void print_cells()
    Input: None
    Output: None
    Purpose: This function is for printing all the data in the hashmap
*/
void print_cells() {
    struct bitmap_hm_data *entry;

    for (entry = g_bm_hm_data; entry != NULL; entry = entry->hh.next) {
        printf("Cell: %d, cardinality: %d\n", entry->bitmap_cell, entry->cardinality);
    }
}