#include "./driver.h"

/*
* File is used to test out parts of code locally. Should avoid pushing to master.
*/

void hashmap_test() {
  char test[100] = "9x";

  printf("Adding hash %s\n", test);
  add_cell(test, 12);

  struct bitmap_hm_data *result = find_cell(test);
  printf("Printing the value retrieved from the hashmap: %d\n", result->cardinality);
  //delete_cell(test);
  unsigned int len = get_hm_length();
  printf("Printing the length of the hashmap: %u\n", len);
  print_cells();
}

int main() {
  hashmap_test();

  return 0;
}
