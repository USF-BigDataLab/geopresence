#include "./driver.h"

int main() {
  const char *geohash_path = "geohashes.txt";
  read_file(geohash_path, 3);
  return 0;
}
