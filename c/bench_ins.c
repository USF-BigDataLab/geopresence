#include "geode.h"
#include "timer.h"
#include "geohash.h"
#include "grid_queries.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_PRECISION 16

struct geode *instances = NULL;

void print_strings_and_free(char **strings) {
    char** save = strings;
    printf("\n");
    for (int i = 0; *(strings) != NULL; i++) {
      printf("%s, ", *strings);
      free(*strings);
      strings++;
    }
    free(save);
}

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

    struct spatial_range *points = (struct spatial_range*) calloc(3, sizeof(struct spatial_range));
	points[0].longitude = -70;
	points[0].latitude = 30;

	points[1].longitude = -85;
	points[1].latitude = 36;

	points[2].longitude = -88;
	points[2].latitude = 34;

    char** poly_res = matching_grid_cells(instances, points, 3);
    print_strings_and_free(poly_res);

    double end = timer_now();
    printf("%f\n", end - start);

    fclose(fp);
    return 0;
}

