#include "./driver.h"

/*
* File is used to test out parts of code locally. Should avoid pushing to master.
* Can Make with 'Make sandbox'
*/


void hashmap_add_test(char *geohash, const int base_prec) {
  //BUG: Key is not being populated correctly
  char key[base_prec+1]; //create key on stack for checking hashmap
  strncpy(key, geohash, base_prec);
  key[base_prec] = '\0';
  printf("Geohash: %s\n", geohash);
  printf("Base precision: %d\n", base_prec);
  printf("Key: %s\n", key);

  struct rbitmap* new_bitmap = init_rbitmap(key, base_prec);

  //NOTE: Key can be on stack
  add_cell(key, new_bitmap);
}

void hashmap_find_test(char *key){
  struct bitmap_hm_data *cell = find_cell(key);
  if(cell == NULL){
    printf("Cell is null\n");
    struct rbitmap* new_bitmap = init_rbitmap(key, 2);
    add_cell(key, new_bitmap);
  } else {
    printf("Cell is not NULL\n");
    printf("Printing the cell retrieved from the hashmap: %s along with latitude: %lf\n",
    cell->bitmap_cell, cell->bmap->gc.latitude);
  }
}

int main() {
  char *geohash = "8gpfwr1b0z";
  const int base_prec = 2;

  hashmap_add_test(geohash, base_prec);
  hashmap_find_test("8p");

  // To check hashmap status
  printf("HASHMAP STATUS\n");
  printf("Length of hashmap: %u\n", get_hm_length());
  print_cells();
  printf("\n");

  return 0;
}
