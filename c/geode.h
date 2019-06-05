#ifndef _GEODE_H_
#define _GEODE_H_

#include "geohash.h"
#include "roaring.h"
#include "uthash.h"

#define PREFIX_SZ 2

struct geode {
    char prefix[PREFIX_SZ + 1];
    struct spatial_range base_range;
    unsigned int width;
    unsigned int height;
    double x_deg;
    double y_deg;
    double x_px;
    double y_px;
    roaring_bitmap_t *bmp;
    UT_hash_handle hh;
};

unsigned int geode_coords_to_idx(struct geode *g, struct spatial_range *sr);
void geode_add_geohash(struct geode *g, const char *geohash);
void geode_add_sprange(struct geode *g, const struct spatial_range *sr);

/**
 * Initializes a new GEODE data structure using the provided base Geohash.
 *
 * This function allocates memory; the resulting GEODE struct should be freed
 * using geode_free().
 *
 * @param base_geohash Geohash to use as the bounding box for this GEODE, e.g.,
 * a base Geohash of 'dj' creates a new GEODE that is centered on the southern
 * USA near Alabama, Georgia, North Florida.
 *
 * @return The initialized GEODE.
 */
struct geode *geode_create(char *base_geohash, unsigned int precision);
void geode_free(struct geode *g);

#endif
