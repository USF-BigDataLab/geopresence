#ifndef _GRID_QUERIES_H_
#define _GRID_QUERIES_H_

#include "geohash.h"
#include "geode.h"

bool has_matching_grid_cells(struct geode *grid, const struct spatial_range *poly, int n);
char** matching_grid_cells(struct geode *grid, const struct spatial_range *poly, int n);

#endif
