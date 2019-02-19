#include "./bitmap.h"

void init_bitmap(roaring_bitmap_t* bitmap){
  bitmap = roaring_bitmap_create();
}

void free_bitmap(roaring_bitmap_t* bitmap){
  roaring_bitmap_free(bitmap);
}

void add(roaring_bitmap_t* bitmap, uint32_t index){
  roaring_bitmap_add(bitmap, index);
}
