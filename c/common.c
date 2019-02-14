#include "common.h"
//Is this the way we should be importing everything? 
//all the basic libraries needed are in geogrid.c, so I
//just imported that. 

/*
    File: common.c 
    Description: This file contains the struct that needs to be 
		 initialized for the bitmap and all the coordinates
		 related to the map. 

		 This file is also used for test cases and a main
		 for running the program.
*/
struct rbitmap* init_rbitmap() {
    struct rbitmap* new_rbitmap = malloc(sizeof(struct rbitmap));
    return new_rbitmap; 
}


int main() {
    printf("Testing :D\n");
    return 0;
}
