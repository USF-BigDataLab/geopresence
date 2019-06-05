#include "geode.h"
#include "timer.h"

#include <stdlib.h>

#define TEST_PRECISION 16

int main(void)
{
    struct geode *g = geode_create("9x", TEST_PRECISION);

    double start = timer_now();
    for (int i = 0; i < 10000000; ++i) {
        int rand_x = (rand() / ((double) RAND_MAX + 1)) * (g->width + 1);
        int rand_y = (rand() / ((double) RAND_MAX + 1)) * (g->height + 1);
        geode_add_xy(g, rand_x, rand_y);
    }
    double end = timer_now();
    printf("%f\n", end - start);
}
