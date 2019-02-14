#include "geogrid.h"
#include "common.h"

/* 
    File: geogrid.c (originally named geoavailabilitygrid in java
	  implementation)
 
    Description: geogrid.c contains the functions related to the 
		 bitmap-based spatial indices. The functions here
		 can be used to determine whether or not data is 
		 stored within a specified spatial region.

    To compile: gcc -std=c11 -o geo -Wall -g geogrid.c geohash.c   

    Please Note: Absolutely need to set the standard to c11, 
	         the files that are included at the top
	         of this file absolutely rely on this config.
*/

/*
    When integrated with the file bitmap.c, we'll have the
    appropriate struct defined up here. 
*/

/*
    struct GeoCoord is the struct name defined in geohash.h; 
    this structis the equivalent to the SpatialRange in the Java code.*/
 
/*
    Function: struct GeoCoord* geo_coord_init()
    Input: String baseGeohash - the geo hash to be decoded
	   and used to generate the new GeoCoord struct. 

	   int precision - used for the calculation of height
			   and width 
    Output: Struct GeoCoord* - A new GeoCoord object
    Description: This function will create 
		 new GeoCoord objects; an init function
		 is necessary for this struct because
		 some work needs to be done upfront 
		 in order to configure it (i.e., get
		 values, decode hash, etc.) 
*/
GeoCoord* geo_coord_init(GeoCoord* gc, char* base_geo_hash, int precision) {
    //See geohash.h and geohash.c for geohash_decode implementation 
    GeoCoord gcs = geohash_decode(base_geo_hash);
    gc = &gcs; //gcs is being point to by the struct passed into the function 

    /*
        height and width calculated by (mentioned in Java code):
	width = 2^(floor(precision/2))
	height = 2^(ceil(precision/2))
    */
    int w = precision/2;
    int h = precision/2;

    if (precision % 2 != 0) {
	h += 1;
    }    

    int width = (1 << w); /* = 2^w */
    int height = (1 << h); /* = 2^h */
    
    /*
        In the Java code, this is a float, and it calculates an
        upperbound and lowerbound; I'm not sure how we'd accomplish
	this in our C implementation since our geohash library doesn't 
	include a way to implement finding the lowerbound and upperbound.

	In the Java impementation, the x_degrees and y_degrees variables
	are floats, but the C version uses doubles as their types 
	because the structs we're using declare these variables as
	doubles. 
     */
    double x_degrees = gc->latitude;
    double y_degrees = gc->longitude; 
    return gc;
}

/*
    Function: bool addPoint() 
    Input: int x, int y - Coordinate to be added
    Output: bool
    Description: This function adds a new point to the
		 geogrid; it looks like the Java version 
		 uses the Coordinates object in the arguments,
		 but we'll pass in int x and int y for now.
		 Coordinates are in latitude and longitude 
		 according to the Java documentation.   
*/
bool addPoint(GeoCoord* gc){
    //How would we access the GeoBoxDimension dimension struct
    //given that it's already inside a struct? 
    if (gc->latitude < 0 || gc->longitude < 0) {
        return false;
    } 

    //if roaring bitmap index == false, then 
    //add index to the pendingUpdates dats structure

    return true; 
}

/*
    Function: xy_to_index(GeoCoord* gc)
    Input: GeoCoord* gc - the struct that the 
	   x and y coordinates will be retrieved
	   from. 
    Output: int
    Description: This converts x and y coordinates to a 
		 specific index within the bitmap. 
*/
int xy_to_index(GeoCoord* gc) {
    //Not sure if the roaring bitmap will have the same
    //scheme as EWAH, but I'll leave this here for now.
    printf("Latitude: %lf\n", gc->latitude);
    printf("Width: %lf\n", gc->dimension.width);
    printf("Longitude: %lf\n", gc->longitude);
   
    int index = (gc->latitude * gc->dimension.width) + gc->longitude;
    return index;
}

/*
    Function: GeoCoord* index_to_GeoCoord(int index) 
    Input: int index - the index that correspond to a coordinate
    Output: GeoCoord *new_geo - a GeoCoord struct that contains the 
	    coordinate found at the index given to the function.	    
    Description: This function takes a given index from the bitmap
		 and finds the coordinate that is at that index. 
*/
GeoCoord* index_to_GeoCoord(int index){
    GeoCoord *new_geo = malloc(sizeof(new_geo)); 
    return new_geo;
}

/*
    Function: GeoCoord* xy_to_spatialrange(int x, int y) 
    Input: int x - x coordinate
	   int y - y coordinate
    Output: GeoCoord* - the new GeoCoord struct that has the new
			x and y coordinates. 
    Description: this function converts x and y coordinates
	         into a GeoCoord struct and returns the new
		 struct.
		 ***This function was called xy_to_spatial_range
		    on the architecture pdf*** 
*/
GeoCoord* xy_to_GeoCoord(int x, int y){
    GeoCoord *new_geo = malloc(sizeof(new_geo));
    return new_geo;    
}

/*
    Function: apply_updates()
    Input: void
    Output: void
    Description: This will apply our updates to the 
		 bitmap.  
*/
void apply_updates(){

}

/*
    Function: void test_function()
    Input: void
    Output: void
    Description: This function is for test cases so that 
		 main doesn't get filled with them.  
*/
void test_function(){
    printf("Hello! Test cases go here\n");
    printf("Initializing a new GeoCoord struct\n");

    GeoCoord *test = malloc(sizeof(test));
    //First argument comes from geohashes.txt file
    //Can't free this for some reason
    test = geo_coord_init(test, "8gpcxc4h3n", 2);

    //The latitude test works fine, but the xy_to_index
    //function just returns 0?
    printf("Latitude: %lf\n", test->latitude);
    printf("Width: %lf\n", test->dimension.width);
    printf("Longitude: %lf\n\n", test->longitude);

    //Why doesn't this function preserver the values in the struct?
    printf("Function test: %d\n", xy_to_index(test));
}

