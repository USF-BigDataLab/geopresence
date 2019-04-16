#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "geogrid.h"
#include "roaring.h"
#include "geohash.h"

/*
    File common.h
    Description: This header file contains on the libraries
		 that need to be imported for the program to work.
		 Additionally, This file declares the struct needed
		 for the bitmap and any coordinates.
*/

/*
    Roaring bitmap struct
    Contains the folowing:
        1) GeoCoord gc - the GeoCoord struct that contains
                          x,y coordinates (north, east, south, west)
                          and height and width (see geohash.h)

        2) roaring_bitmap_t *rbp - the roaring bitmap that we can use
           with the function in roaring.c
*/

struct rbitmap {
    GeoCoord gc; //Base geocoord
    roaring_bitmap_t *rbp; //Roaring bit map
};

struct rbitmap* init_rbitmap();
void free_rbitmap(struct rbitmap* f_rbitmap);
void rbitmap_add_all(struct rbitmap *bitmap, const char *file_path, int precision);
void rbitmap_add_all_buff(struct rbitmap *bmp, const char *file_path, int precision);
void test();
void value_ptr_test();
void coord_insertion_test();
bool addPoint(struct rbitmap* bitmap);
GeoCoord index_to_GeoCoord(int index, GeoCoord orig_coord);
void index_to_geo_test();
void xy_to_index_test();
#endif
