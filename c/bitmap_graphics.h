#ifndef _BITMAP_GRAPHICS_H_
#define _BITMAP_GRAPHICS_H_

void bmp_draw_line (roaring_bitmap_t *bmp, int x1, int y1, int x2, int y2, int width);
void bmp_filled_polygon (roaring_bitmap_t *bmp, geodePointPtr p, int n, int width);

#endif
