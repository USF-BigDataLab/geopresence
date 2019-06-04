#include "geode.h"
#include "timer.h"

#define TEST_PRECISION 16

int main(void)
{
    FILE *fp = fopen("../datasets/geohashes.txt", "r");
    struct geode *instances = NULL;

    char line[128];
    double start = timer_now();
    while (fgets(line, 128, fp) != NULL) {
        char prefix[PREFIX_SZ + 1] = { '\0' };
        memcpy(prefix, line, PREFIX_SZ);

        struct geode *instance;
        HASH_FIND_STR(instances, prefix, instance);
        if (instance == NULL) {
            instance = geode_create(line, TEST_PRECISION);
            HASH_ADD_STR(instances, prefix, instance);
        }

        struct spatial_range sr;
        geohash_decodeN(&sr, line);

        /* TODO: this should be done in a function, probably geode_add() */
        unsigned int idx = geode_coords_to_idx(instance, &sr);
        roaring_bitmap_add(instance->bmp, idx);
    }
    double end = timer_now();
    printf("%f\n", end - start);

    fclose(fp);
}
