#ifndef _GEOGRID_H_
#define _GEOGRID_H_

#include "common.h"
#include "geohash.h"
#include <stdbool.h>

//See geogrid.c for function descriptions

void print_gc(GeoCoord gc);
GeoCoord geo_coord_init(GeoCoord gc, char* base_geo_hash, int precision);
GeoCoord hash_to_geo(char* base_geo_hash, int precision);
//bool addPoint(struct rbitmap* bitmap);
int xy_to_index(GeoCoord gc);
/*
GeoCoord* index_to_GeoCoord(int index);
GeoCoord* xy_to_GeoCoord(int x, int y);
void apply_updates();
*/
#endif
