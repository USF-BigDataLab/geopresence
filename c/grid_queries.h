#ifndef _GRID_QUERIES_H_
#define _GRID_QUERIES_H_

#include "geohash.h"
#include "geode.h"

char** matching_grid_cells(struct geode *grid, GeoCoord *bbox);
bool has_matching_grid_cells(struct geode *grid, GeoCoord *bbox);
char** matching_grid_cells_polygon(struct geode *grid, const struct spatial_range *coords, int n);

#endif
