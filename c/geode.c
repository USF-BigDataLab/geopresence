
#include <math.h>
#include <stdio.h>

#include "log.h"
#include "geode.h"
#include "roaring.c"
#include "uthash.h"
#include "geohash.h"

#define PRECISION 16

struct geode *geode_create(char *base_geohash, unsigned int precision)
{
    struct geode *g = malloc(sizeof(struct geode));
    strncpy(g->prefix, base_geohash, PREFIX_SZ);
    g->prefix[PREFIX_SZ] = '\0';
    g->bmp = roaring_bitmap_create();

    g->base_geohash = geohash_decode(g->prefix);

    /*
     * height, width calculated like so:
     * width = 2^(floor(precision / 2))
     * height = 2^(ceil(precision / 2))
     */
    int w = precision / 2;
    int h = precision / 2;
    if (precision % 2 != 0) {
        h += 1;
    }

    g->width = (1 << w); /* = 2^w */
    g->height = (1 << h); /* = 2^h */

    /* Determine the number of degrees in the x and y directions for the
     * base spatial range this geoavailability grid represents */
    g->x_deg = fabs(g->base_geohash.west - g->base_geohash.east);
    g->y_deg = fabs(g->base_geohash.north - g->base_geohash.south);

    /* Determine the number of degrees represented by each grid pixel */
    g->x_px = g->x_deg / (double) g->width;
    g->y_px = g->y_deg / (double) g->width;

    LOG("New GEODE: %s (%d x %d), dpp: (%f, %f)\n",
            g->prefix,
            g->width,
            g->height,
            g->x_px,
            g->y_px);

    return g;
}

/**
 * Converts X, Y coordinates to a particular index within the underlying
 * bitmap implementation.  Essentially this converts a 2D index to a 1D
 * index.
 *
 * @param x The x coordinate to convert
 * @param y The y coorddinate to convert
 *
 * @return A single integer representing the bitmap location of the X, Y
 * coordinates.
 */
int geode_xy_to_index(struct geode *g, int x, int y) {
    return y * g->width + x;
}

void print_geocoord(GeoCoord *gc)
{
    printf("(%f, %f) in [%f, %f, %f, %f]\n", gc->longitude, gc->latitude, gc->north, gc->east, gc->south, gc->west);
}

/**
 * Converts a coordinate pair (defined with latitude, longitude in decimal
 * degrees) to an x, y location in the grid.
 *
 * @param coords the Coordinates to convert.
 *
 * @return Corresponding x, y location in the grid.
 */
unsigned int geode_coords_to_idx(struct geode *g, GeoCoord *gc) {

    /* Assuming (x, y) coordinates for the geoavailability grids, latitude
     * will decrease as y increases, and longitude will increase as x
     * increases. This is reflected in how we compute the differences
     * between the base points and the coordinates in question. */
    float xDiff = fabs(gc->longitude - g->base_geohash.west);
    float yDiff = fabs(gc->latitude - g->base_geohash.north);

    int x = (int) (xDiff / g->x_px);
    int y = (int) (yDiff / g->y_px);

    return y * g->width + x;
}

int main(void)
{
    geo_init_map();
    FILE *fp = fopen("../datasets/geohashes.txt", "r");
    struct geode *instances = NULL;

    char line[128];
    while(fgets(line, 128, fp) != NULL) {
//        strcpy(line, "9xbpb");
        char prefix[PREFIX_SZ + 1];
        strncpy(prefix, line, PREFIX_SZ);

        struct geode *instance;
        HASH_FIND_STR(instances, prefix, instance);
        if (instance == NULL) {
            instance = geode_create(line, 16);
            HASH_ADD_STR(instances, prefix, instance);
        }

        GeoCoord gc = geohash_decode(line);
        //print_geocoord(&gc);
        unsigned int idx = geode_coords_to_idx(instance, &gc);
        roaring_bitmap_add(instance->bmp, idx);

    }

//    struct geode *inst;
//    for (inst = instances; inst != NULL; inst = inst->hh.next) {
//        uint32_t c = roaring_bitmap_get_cardinality(inst->bmp);
//        printf("-> %s %d\n", inst->prefix, c);
//    }
}
