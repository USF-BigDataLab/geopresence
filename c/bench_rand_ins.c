#include "geode.h"
#include "timer.h"

#include <stdlib.h>

#define TEST_PRECISION 28

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr,
                "Usage: %s insertions\n"
                "Ex: %s 1000000\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    long insertions = atol(argv[1]);
    printf("Inserting %ld random points\n", insertions);

    struct geode *g = geode_create("9x", TEST_PRECISION);

    double start = timer_now();
    for (int i = 0; i < insertions; ++i) {
        int rand_x = (rand() / ((double) RAND_MAX + 1)) * (g->width + 1);
        int rand_y = (rand() / ((double) RAND_MAX + 1)) * (g->height + 1);
        geode_add_xy(g, rand_x, rand_y);
    }
    double end = timer_now();
    printf("%f\n", end - start);

    printf("Cardinality = %d\n",
            (int) roaring_bitmap_get_cardinality(g->bmp));
}
