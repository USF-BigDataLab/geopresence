#ifndef _GEOGRID_H_
#define _GEOGRID_H_

#include "common.h"
#include "geohash.h"
#include <stdbool.h>

//See geogrid.c for function descriptions

void print_gc(GeoCoord gc);
GeoCoord geo_coord_init(GeoCoord gc, char* base_geo_hash, int precision);
GeoCoord hash_to_geo(char* base_geo_hash, int precision);
int geohash_to_index(GeoCoord base_coords, char *geohash);
//bool addPoint(struct rbitmap* bitmap);
int xy_to_index(GeoCoord gc);
int coords_to_index(GeoCoord base_coords, GeoCoord t_coord);
GeoCoord index_to_GeoCoord(int index, GeoCoord orig_coord);
/*
GeoCoord* xy_to_GeoCoord(int x, int y);
*/
#endif
