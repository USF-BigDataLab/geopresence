#include "./driver.h"
#include "sandbox.c"

int main() {
  const char *geohash_path = "geohashes.txt";
  read_file(geohash_path, 3);
  //hashmap_test();
  return 0;
}
