#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "geogrid.h"
#include "roaring.h"

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
        1) GeoCoord *gc - the GeoCoord struct that contains 
                          x,y coordinates (north, east, south, west)
                          and height and width (see geohash.h)

        2) roaring_bitmap_t *rbp - the roaring bitmap that we can use
           with the function in roaring.c 
*/
struct rbitmap {
    GeoCoord *gc; //GeoCoord struct
    roaring_bitmap_t *rbp; //Roaring bit map
};
#endif
