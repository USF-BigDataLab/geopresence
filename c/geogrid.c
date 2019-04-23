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


void print_gc(GeoCoord gc){
  printf("Latitude: %lf\tLongitude: %lf\nHeight: %lf\tWidth: %lf\n North: %lf\n East: %lf\n South: %lf\n West: %lf\n",
  gc.latitude, gc.longitude, gc.dimension.height, gc.dimension.width, gc.north, gc.east, gc.south, gc.west);
}

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
GeoCoord geo_coord_init(GeoCoord gc, char* base_geo_hash, int precision) {
    //See geohash.h and geohash.c for geohash_decode implementation
    //So, GeoCoord is NOT a pointer because the geohash_decode function will return a
    //GeoCoord struct; since gcs is NOT a pointer, memcpy will be called when assigning
    //the return value of geohash_decode to the struct gcs.

    //Keep in mind that latitude is x (or length) and longitude is y (or height)
    gc = geohash_decode(base_geo_hash);

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

    gc.dimension.width = (1 << w); /* = 2^w */
    gc.dimension.height = (1 << h); /* = 2^h */

    //Might need to add width and height to the GeoCoord struct

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

    //These variables are used for pixel calculations later (see Java implementation)
    //double x_degrees = gc.latitude;
    //double y_degrees = gc.longitude;
    return gc;
}

/*
    Function: hash_to_geo
    Creates a GeoCoord from a hash and precision
    Input:
      - baseGeohash: the geo hash to be converted to GeoCoord struct
      - precision: used for the calculation of height and width

    Returns: A GeoCoord created from the hash and precision
*/
GeoCoord hash_to_geo(char* base_geo_hash, int precision){

  GeoCoord gc = geohash_decode(base_geo_hash);

  int w = precision/2;
  int h = precision/2;

  if (precision % 2 != 0) {
    h += 1;
  }

  gc.dimension.width = (1 << w); /* = 2^w */
  gc.dimension.height = (1 << h); /* = 2^h */

  return gc;
}

/*
    Function: xy_to_index(GeoCoord gc)
    Input: GeoCoord gc - the struct that the
	   x and y coordinates will be retrieved
	   from.
    Output: int
    Description: This converts x and y coordinates to a
		 specific index within the bitmap.
*/
int xy_to_index(GeoCoord gc) {
    int index = (gc.longitude * gc.dimension.width) + gc.latitude;
    return index;
}

/*
* Function: geo_to_index
* Create an index relative to base GeoCoord.
*/
int geo_to_index(GeoCoord base_geo, GeoCoord geo){
  return (geo.latitude * base_geo.dimension.width) + geo.longitude;
}

int coord_to_index(GeoCoord base_coords, GeoCoord t_coord){
  float x_diff = t_coord.longitude - base_coords.west;
  float y_diff = t_coord.latitude - base_coords.south;

  int y = y_diff; //should use y_degree_per_pixel?
  int x = x_diff;
  int width = base_coords.dimension.width;

  return (int) (y * width) + x; //algorithm to convert 2D to 1D
}

// Removing once done fixing indexing
// The result file uniq word count should match the number of geohashes read in
void test_indexing(){
  GeoCoord base_geo = geohash_decode("8");
  base_geo.dimension = geohash_dimensions_for_precision(1);
  print_gc(base_geo);

  FILE *geo_fp, *r_fp;
  char buff[255];

  geo_fp = fopen("./8-geohashes.txt", "r"); //hashes only in the 8 region
  r_fp = fopen("8-indexes.txt", "w");
  while(fgets(buff, 255, (FILE *) geo_fp)){
    GeoCoord temp_geo = geohash_decode(buff);
    int index = coord_to_index(base_geo, temp_geo);
    fprintf(r_fp, "%d\t%lf\t%lf\n", index, temp_geo.latitude, temp_geo.longitude);
  }
  fclose(geo_fp);
  fclose(r_fp);
}

/*
    Function: GeoCoord index_to_GeoCoord(int index)
    Input: int index - the index that correspond to a coordinate
    Output: GeoCoord new_geo - a GeoCoord struct that contains the
	    coordinate found at the index given to the function.
    Description: This function takes a given index from the bitmap
		 and finds the coordinate that is at that index. This is called
         index_to_xy in the Java implementation, but it's called index_to_GeoCoord
         in the C version since the Point object doesn't exist in C.
*/
GeoCoord index_to_GeoCoord(int index, GeoCoord orig_coord) {
    GeoCoord new_geo = orig_coord;
    new_geo.latitude = index % (int) (orig_coord.longitude); //has to be an int for modulus operator
    new_geo.longitude = index / orig_coord.longitude;
    return new_geo;
}

/*
    Function: GeoCoord xy_to_spatialrange(int x, int y)
    Input: int x - x coordinate
	   int y - y coordinate
    Output: GeoCoord - the new GeoCoord struct that has the new
			x and y coordinates.
    Description: this function converts x and y coordinates
	         into a GeoCoord struct and returns the new
		 struct.
		 ***This function was called xy_to_spatial_range
		    on the architecture pdf***

    NOTICE: Not completely sure how we implement this at the moment given
            that the Java implementation gets lowerbounds for both longitude
            and latitude. Also, the Java implementation uses degrees per pixel
            variables, which haven't been implemented here yet since we mentioned
            that we won't focus on anything involving actually graphing the data
*/
GeoCoord xy_to_GeoCoord(int x, int y) {
    GeoCoord new_geo = {0};
    return new_geo;
}
