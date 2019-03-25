#include "./common.h"
#define INDEXES_SIZE 1000
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
    new_rbitmap->rbp = roaring_bitmap_create();

    //new_rbitmap->gc = malloc(sizeof(GeoCoord*));
    // new_rbitmap->gc = geo_coord_init(new_rbitmap->gc, "8gpguuck7u", 5);
    // print_gc(new_rbitmap);

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
    //free(f_rbitmap->gc);
    free(f_rbitmap->rbp);
    free(f_rbitmap);
}

/*
    Function: rbitmap_add_all
    Populates bitmap from geohashes converted into indexes from a file.
    Input:
      - bmp: rbitmap to populate
      - file_path: txt file with geohashes
      - precision: Used by GeoCoord to calculate heigh and width
    Returns: Void
*/
void rbitmap_add_all(struct rbitmap *bmp, char *file_path, int precision){
  // can test w/ "../datasets/geohashes.txt"

  FILE *fp;
  char buff[255];
  GeoCoord temp_gc;

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){
    // printf("%s", buff);

    temp_gc = hash_to_geo(buff, precision);
    int index = xy_to_index(temp_gc);

    roaring_bitmap_add(bmp->rbp, index);
    // printf("Cardinality = %llu \n", roaring_bitmap_get_cardinality(bmp->rbp));
  }

  fclose(fp);
}

/*
    Function: rbitmap_add_all_buff
    Populates bitmap from geohashes converted into indexes from a file while
    using an index buffer to use Roarings add_many function which claims to
    be faster.
    Input:
      - bmp: rbitmap to populate
      - file_path: txt file with geohashes
      - precision: Used by GeoCoord to calculate heigh and width
    Returns: Void
*/
void rbitmap_add_all_buff(struct rbitmap *bmp, char *file_path, int precision){
  FILE *fp;
  char buff[255];
  //const int indexes_size = 1000;
  int indexes[INDEXES_SIZE];
  int i = 0; //index buffer
  GeoCoord temp_gc;

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){
    // printf("%s", buff);

    temp_gc = hash_to_geo(buff, precision);
    int index = xy_to_index(temp_gc);
    indexes[i++] = index;

    if(i >= INDEXES_SIZE-1){
      roaring_bitmap_add_many(bmp->rbp, INDEXES_SIZE, &indexes);
      i = 0; //reset
    }

    // printf("Cardinality = %llu \n", roaring_bitmap_get_cardinality(bmp->rbp));
  }

  if(i > 0) //add any leftover indexes
    roaring_bitmap_add_many(bmp->rbp, i, &indexes);

  // printf("Cardinality = %llu \n", roaring_bitmap_get_cardinality(bmp->rbp));

  fclose(fp);
}

/*
     Function: void test()
     Input: None
     Output: None
     Description: This function is used for testing initialization and freeing
*/
void init_and_free_test() {
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

/*
    Function: void value_ptr_test()
    Input: None
    Output: None
    Description: This function is used for testing values of the struct.
*/
void value_ptr_test(){
    printf("**************************************************\n");
    printf("Starting initialize bitmap test\n");
    printf("--------------------------------------------------\n");
    struct rbitmap* test = init_rbitmap();
    printf("Sucessfully initialized bitmap\n");
    printf("**************************************************\n\n");

    printf("**************************************************\n");
    printf("Accessing values in the bitmap\n");
    printf("--------------------------------------------------\n");
    printf("GeoCoord info: \n");
    printf("Latitude: %lf    Longitude: %lf\nHeight: %lf    Width: %lf\n",
    test->gc.latitude, test->gc.longitude, test->gc.dimension.height, test->gc.dimension.width);
    printf("**************************************************\n");
}

/*
    Function: void coord_insertion_test()
    Input: None
    Output: None
    Description: This function is used for testing the insertion function specified in
    geogrid.c
*/
void coord_insertion_test(){
    printf("**************************************************\n");
    printf("Starting initialize bitmap test\n");
    printf("--------------------------------------------------\n");
    struct rbitmap* test = init_rbitmap();
    printf("Sucessfully initialized bitmap\n");
    printf("**************************************************\n\n");

    printf("**************************************************\n");
    printf("Testing addPoint function\n");
    printf("--------------------------------------------------\n");
    addPoint(test);
}

/*
  Function: void insertion_benchmark()
  Input: None
  Output: None
  Description: This function is used specifcally for benchmarking; this will
  allow us to see the time performance of inserting points into the roaring
  bitmap. 
*/
void insertion_benchmark() {
    printf("STARTING INSERTION BENCHMARK\n");
    clock_t start, end;
    double time_taken;
    start = clock();

    struct rbitmap* test = init_rbitmap();
    char* filename;
    filename = "geohashes.txt";
    rbitmap_add_all_buff(test, filename, 12);

    // Custom iterator that the developers of CRoaring created
    int counter = 0;
    roaring_uint32_iterator_t * i = roaring_create_iterator(test->rbp);

    while (i->has_value) {
      printf("current value: %d\n", i->current_value); // Printing value at the given index
      counter++;
      roaring_advance_uint32_iterator(i);
    }

    roaring_free_uint32_iterator(i); // Free the iterator

    end = clock();
    time_taken = ((double) (end - start) / CLOCKS_PER_SEC);
    printf("Total time (seconds): %f\n", time_taken);
}
