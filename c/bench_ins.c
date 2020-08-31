#include "geode.h"
#include "timer.h"
#include "geohash.h"
#include "grid_queries.h"
#include <math.h>

#define TEST_PRECISION 16

struct geode *instances = NULL;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr,
                "Usage: %s geohash_file.txt\n"
                "Ex: %s ../datasets/geohashes.txt\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    printf("Opening %s\n", argv[1]);
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

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

        geode_add_geohash(instance, line);
    }

    char *test = "d";
    
    GeoCoord coord = geohash_decode(test);
    coord.north = 44;
    coord.east = -66;
    coord.south = 30;
    coord.west = -89;
    char** res = matching_grid_cells(instances, &coord);
    matching_grid_cells_polygon(instances);

    char** save = res;

    for (int i = 0; *(res) != NULL; i++) {
      printf("%s, ", *res);
      free(*res);
      res++;
    }
    free(save);


    double end = timer_now();
    printf("%f\n", end - start);

    fclose(fp);
}
