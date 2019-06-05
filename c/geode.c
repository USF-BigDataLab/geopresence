#include <math.h>
#include <stdio.h>

#include "log.h"
#include "geode.h"
#include "roaring.c"
#include "uthash.h"
#include "geohash.h"

void geode_add_geohash(struct geode *g, const char *geohash)
{
    struct spatial_range sr;
    geohash_decodeN(&sr, geohash);
    geode_add_sprange(g, &sr);
}

void geode_add_sprange(struct geode *g, const struct spatial_range *sr)
{
    unsigned int idx = geode_sprange_to_idx(g, sr);
    roaring_bitmap_add(g->bmp, idx);
}

struct geode *geode_create(char *base_geohash, unsigned int precision)
{
    struct geode *g = malloc(sizeof(struct geode));
    if (g == NULL) {
        perror("malloc");
        return NULL;
    }

    memcpy(g->prefix, base_geohash, PREFIX_SZ);
    g->prefix[PREFIX_SZ] = '\0';
    g->bmp = roaring_bitmap_create();

    geohash_decodeN(&g->base_range, g->prefix);

    /*
     * height, width calculation:
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

    /* Determine the number of degrees in the x and y directions for the base
     * spatial range this GEODE represents */
    g->x_deg = fabs(g->base_range.west - g->base_range.east);
    g->y_deg = fabs(g->base_range.north - g->base_range.south);

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
int geode_xy_to_idx(struct geode *g, const int x, const int y)
{
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
unsigned int geode_sprange_to_idx(
        struct geode *g, const struct spatial_range *sr)
{

    /* Assuming (x, y) coordinates for the geoavailability grids, latitude
     * will decrease as y increases, and longitude will increase as x
     * increases. This is reflected in how we compute the differences
     * between the base points and the coordinates in question. */
    float xDiff = fabs(sr->longitude - g->base_range.west);
    float yDiff = fabs(sr->latitude - g->base_range.north);

    const int x = (int) (xDiff / g->x_px);
    const int y = (int) (yDiff / g->y_px);

    return geode_xy_to_idx(g, x, y);
}

//    struct geode *inst;
//    for (inst = instances; inst != NULL; inst = inst->hh.next) {
//        uint32_t c = roaring_bitmap_get_cardinality(inst->bmp);
//        printf("-> %s %d\n", inst->prefix, c);
//    }
