#ifndef _GRID_QUERIES_H_
#define _GRID_QUERIES_H_

#include "geohash.h"
#include "geode.h"

char** geodes(struct geode *grid, const struct spatial_range *poly, int n);
char** matching_geodes(struct geode *grid, const struct spatial_range *poly, int n);
bool has_matching_geodes(struct geode *grid, const struct spatial_range *poly, int n);

#endif
