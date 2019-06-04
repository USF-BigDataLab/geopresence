#include "geode.h"
#include "timer.h"

#include <stdlib.h>

#define TEST_PRECISION 16

int main(void)
{
    struct geode *g = geode_create("9x", 20);

    double start = timer_now();
    for (int i = 0; i < 10000000; ++i) {
        int rand_x = (rand() / ((double) RAND_MAX + 1)) * (g->width + 1);
        int rand_y = (rand() / ((double) RAND_MAX + 1)) * (g->height + 1);

        /* TODO: this should be done in a function, probably geode_add() */
        unsigned int idx =  rand_y * g->width + rand_x;
        roaring_bitmap_add(g->bmp, idx);
    }
    double end = timer_now();
    printf("%f\n", end - start);
}
