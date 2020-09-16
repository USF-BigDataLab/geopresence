#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <malloc/_malloc.h>
#include <math.h>
#include <stdio.h>

#include "log.h"
#include "geode.h"
#include "roaring.c"
#include "uthash.h"
#include "geohash.h"
#include "bitmap_graphics.h"

static void query_transform(roaring_bitmap_t *r, struct geode *g, const struct spatial_range *coords, int n);

/**
 * Create a geode with a base hash and precision of bitmap
 *
 * @param base_geohash - starting hash
 * @param print_geode  - precision of bitmap for the geode
 */
struct geode *geode_create(char *base_geohash, unsigned int precision)
{
    struct geode *g = malloc(sizeof(struct geode));
    if (g == NULL) {
        perror("malloc");
        return NULL;
    }

    memcpy(g->prefix, base_geohash, PREFIX_SZ);
    g->prefix[PREFIX_SZ] = '\0';

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

    g->bmp = roaring_bitmap_create_with_capacity(g->width * g->height);

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

void geode_add_xy(struct geode *g, const int x, const int y)
{
    unsigned int idx = geode_xy_to_idx(g, x, y);
    roaring_bitmap_add(g->bmp, idx);
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

/**
 * Prints a geocoord
 */
void print_geocoord(GeoCoord *gc)
{
    printf("(%f, %f) in [%f, %f, %f, %f]\n", gc->longitude, gc->latitude, gc->north, gc->east, gc->south, gc->west);
}

/** 
 * Prints geode as above
 */
void print_geode(struct geode *gc)
{
    printf("%s (%f, %f) in [%f, %f, %f, %f]\n", gc->prefix, gc->base_range.longitude, gc->base_range.latitude, gc->base_range.north, gc->base_range.east, gc->base_range.south, gc->base_range.west);
}

void print_pbm(roaring_bitmap_t *bmp, unsigned int x, unsigned int y, char *file_name) {

  FILE *fp = fopen(file_name, "w");
  
  char header[128];

  fputs("P1\n", fp);
  int null_pos = sprintf(header, "%u %u\n", x, y);
  header[null_pos] = '\0';
  fputs(header, fp);

  for (int i = 0; i < x; i++){
    for (int j = 0; j < y; j++){
      roaring_bitmap_contains(bmp, i*x + j) ? fputs("1", fp) : fputs("0", fp);
      fputs(" ", fp);
    }
      fputs("\n", fp);
  }
  fclose(fp);
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

/**
 * Converts a coordinate pair (defined with latitude, longitude in decimal
 * degrees) to an x, y location in the grid.
 *
 * @param coords the Coordinates to convert.
 *
 * @return Corresponding x, y location in the grid.
 */
geodePoint geode_coord_to_xy(
        struct geode *g, const struct spatial_range *sr)
{

    /* Assuming (x, y) coordinates for the geoavailability grids, latitude
     * will decrease as y increases, and longitude will increase as x
     * increases. This is reflected in how we compute the differences
     * between the base points and the coordinates in question. */
    float xDiff = sr->longitude - g->base_range.west;
    float yDiff = g->base_range.north - sr->latitude;

    geodePoint point;
    point.x = (int) (xDiff / g->x_px);
    point.y = (int) (yDiff / g->y_px);
    return point;
}

/**
 * Function: polygon_intersects_geode
 */
bool polygon_intersects_geode(struct geode *g, const struct spatial_range *coords, int n, int count) {
    roaring_bitmap_t *r = roaring_bitmap_create_with_capacity(g->width * g->height);


    query_transform(r, g, coords, n);
    
    char f_whole[128];
    char f_query[128];

    *(f_whole + sprintf(f_whole, "./pbm/poly_whole_%s.pbm", g->prefix)) = '\0';
    *(f_query + sprintf(f_query, "./pbm/poly_query_%s.pbm", g->prefix)) = '\0';
    print_pbm(r, g->width, g->height, f_query);
    print_pbm(g->bmp, g->width, g->height, f_whole);

    return roaring_bitmap_intersect(g->bmp, r);
}

/**
 * Function: polygon_query_geode
 * Purpose:  get the index locations that are set in g and contained in the bounds of coords
 * Params:   g      - the geode to query
 *           coords - the polygon represented as lat/lon pairs
 *           n      - number of lat/lon pairs
 *
 * Return:   locations_and_sz - array of matching locations and the size of the array.
 *                              the size is determined with roaring_bitmap_get_cardinality
 */
struct query_result* polygon_query_geode(struct geode *g, const struct spatial_range *coords, int n) {
    /* Transform the polygon query into a roaring_bitmap_t */
    roaring_bitmap_t *r = roaring_bitmap_create_with_capacity(g->width * g->height);
    query_transform(r, g, coords, n);
   
    /* convert the intersecting grid cells of the geode and the query to and array of matching index locations */
    roaring_bitmap_and_inplace(r, g->bmp);
    uint64_t sz = roaring_bitmap_get_cardinality(r);
    uint32_t *locations = calloc(sz, sizeof(uint32_t));
    roaring_bitmap_to_uint32_array(r, locations);

    /* expose the number of index locations in the array */
    struct query_result *locations_and_sz = malloc(sizeof(struct query_result));
    locations_and_sz->locations = locations;
    locations_and_sz->sz = sz;

    return locations_and_sz;
}


/**
 * Function: query_transform
 *
 * Purpose:  Resolve the set of latitude/longitude coordinates in the query 
 *           with the coordinates of the geode. 
 *
 *           Convert each point to an xy location in the bitmap, 
 *           then draw the filled polygon with the set of xy pairs.
 *
 * @param    r - The bitmap to draw the polygon query into
 *           g - The geode that the bitmap should be drawn for. If any part of the polygon is
 *               outside this geode's spatial range, that part of the polygon will be clipped
 *           c - Set of latitude and longitude coordinates
 *           n - number of coordinates
 */
void query_transform(roaring_bitmap_t *r, struct geode *g, const struct spatial_range *c, int n) {
    geodePointPtr points = (geodePointPtr)calloc(n, sizeof(geodePoint));
    for (int i = 0; i < n; i++) {
        points[i] = geode_coord_to_xy(g, &c[i]);
    }
    bmp_filled_polygon(r, points, n, g->width, g->height);
    free(points);
}
