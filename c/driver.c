#include "./driver.h"

int main() {
  const char *geohash_path = "geohashes.txt";
  int count = read_file(geohash_path, 3);

  // printf("Length of hashmap: %u\n", get_hm_length());
  // print_cells();
  printf("Read %d hashes\n", count);
  return 0;
}
