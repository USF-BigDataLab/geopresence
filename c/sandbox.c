#include "./driver.h"

/*
* File is used to test out parts of code locally. Should avoid pushing to master.
*/

void hashmap_test() {
  char test[100] = "9x";
  struct rbitmap* new_bitmap = init_rbitmap("8gpcxc4h3n", 12);

  printf("Adding hash %s\n", test);
  add_cell(test, new_bitmap);

  struct bitmap_hm_data *result = find_cell(test);
  printf("Printing the cell retrieved from the hashmap: %s along with latitude: %lf\n", result->bitmap_cell, result->bmap->gc.latitude);
  //delete_cell(test);
  unsigned int len = get_hm_length();
  printf("Printing the length of the hashmap: %u\n", len);
  print_cells();
}
