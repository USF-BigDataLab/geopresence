#include "./common.h"

/*
    File: common.c
    Description: This file contains the struct that needs to be
		 initialized for the bitmap and all the coordinates
		 related to the map.

		 This file is also used for test cases and a main
		 for running the program.
*/

/*
    Function: struct rbitmap* init_rbitmap()
    Input: None
    Output: struct rbitmap* - returns a pointer to the new roaring bitmap
			      that was allocated.
    Description: This function initializes the roaring bitmap by
		 dynamically allocating all the memory needed.
		 See common.h for the details of struct rbitmap
*/
struct rbitmap* init_rbitmap() {
    struct rbitmap* new_rbitmap = malloc(sizeof(struct rbitmap));
    new_rbitmap->gc = malloc(sizeof(GeoCoord*));
    new_rbitmap->gc = geo_coord_init(new_rbitmap->gc, "8gpcxc4h3n", 2);
    new_rbitmap->rbp = roaring_bitmap_create();
    return new_rbitmap;
}

/*
    Function: void free_rbitmap(struct rbitmap* f_rbitmap)
    Input: struct rbitmap* f_rbitmap - the bitmap to be freed
    Output: None
    Description: Frees all the components of the struct passed
		 to the function.
*/
void free_rbitmap(struct rbitmap* f_rbitmap) {
    free(f_rbitmap->gc);
    free(f_rbitmap->rbp);
    free(f_rbitmap);
}

void init_rbitmap_from_file(char *file_path){
  // can test w/ "../datasets/geohashes.txt"

  FILE *fp;
  char buff[255];

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){
    /* TODO: Populate bitmap */
    // printf("%s", buff);
  }

  fclose(fp);
}

/*
     Function: void test()
     Input: None
     Output: None
     Description: This function is used for test cases.
*/
void test(){
    printf("TEST FUNCTION CALLED\n");
    printf("**************************************************\n\n");

    printf("**************************************************\n");
    printf("Starting initialize bitmap test\n");
    printf("--------------------------------------------------\n");
    struct rbitmap* test = init_rbitmap();
    printf("Sucessfully initialized bitmap\n");
    printf("**************************************************\n\n");

    printf("**************************************************\n");
    printf("Starting bitmap memory free test\n");
    printf("--------------------------------------------------\n");
    free_rbitmap(test);
    printf("Successfully freed bitmap\n");
    printf("**************************************************\n\n");
}
