#include "./driver.h"

int main() {
  const char *geohash_path = "geohashes.txt";
  // struct bitmap_hm_data *g_bm_hm_data = NULL;
  read_file(geohash_path, 3);

  return 0;
}
