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
    Input:
        - base_geohash: Decoded to base geo coord
        _ base_precision: Length of geohash passed; used for dimensions
    Output: struct rbitmap* - returns a pointer to the new roaring bitmap
			      that was allocated.
    Description: This function initializes the roaring bitmap by
		 dynamically allocating all the memory needed.
		 See common.h for the details of struct rbitmap
*/
struct rbitmap* init_rbitmap(char *base_geohash, int base_precision) {
    struct rbitmap* new_rbitmap = malloc(sizeof(struct rbitmap));
    new_rbitmap->rbp = roaring_bitmap_create();

    new_rbitmap->gc = geohash_decode(base_geohash);
    new_rbitmap->gc.dimension = geohash_dimensions_for_precision(base_precision);

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
* Function: read_file
* Goes through a file of geohashes to create bitmaps of each region.
* Input:
*     - file_path: Path to file
*     - base_prec: Precion for base geo-coordinates struct
* Returns: -1 if input error; if no errors returns count of geohashes read
*/
int read_file(const char *file_path, const int base_prec){

  if(base_prec < 1){
    printf("ERROR: Base precision must be a positive integer\n");
    return -1;
  }

  FILE *fp;
  char key[base_prec + 1];
  char buff[255];

  int index, count = 0;
  struct rbitmap* bmp = NULL;

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){
    strncpy(key, buff, base_prec);
    printf("Checking if %s is in hashmap ...\n", key);

    struct bitmap_hm_data *cell = find_cell(key);
    if(cell == NULL){
      printf("Not in hashmap, creating a new bitmap\n");

      struct rbitmap *bmp = init_rbitmap(key, base_prec);
      add_cell(key, bmp);
    }
    else {
      printf("In hashmap, updating bitmap\n");
      bmp = cell->bmap;
    }

    index = geohash_to_index(bmp->gc, buff);
    roaring_bitmap_add(bmp->rbp, index);
    count += 1;
  }
  printf("Length of hashmap: %u\n", get_hm_length());
  // print_cells();

  fclose(fp);
  return count;
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
/*
void rbitmap_add_all(struct rbitmap *bmp, const char *file_path, int precision){
  FILE *fp;
  char buff[255];
  GeoCoord temp_gc;
  int index;

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){

    // index = geohash_to_index(bmp.gc, buff)
    // roaring_bitmap_add(bmp->rbp, index);
  }
  fclose(fp);
}
*/


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
/*
void rbitmap_add_all_buff(struct rbitmap *bmp, const char *file_path, int precision){
  FILE *fp;
  char buff[255];
  int indexes[INDEXES_SIZE];
  int i = 0; //index buffer
  GeoCoord temp_gc;

  fp = fopen(file_path, "r");
  while(fgets(buff, 255, (FILE*) fp)){

    temp_gc = hash_to_geo(buff, precision);
    int index = xy_to_index(temp_gc);
    indexes[i++] = index;

    if(i >= INDEXES_SIZE-1){
      roaring_bitmap_add_many(bmp->rbp, INDEXES_SIZE, &indexes);
      i = 0; //reset
    }
  }

  if(i > 0) //add any leftover indexes
    roaring_bitmap_add_many(bmp->rbp, i, &indexes);

  fclose(fp);
}
*/
