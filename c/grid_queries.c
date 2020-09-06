#include <stdio.h>
#include <stdlib.h>
#include "geode.h"
#include "geohash.h"
#include "grid_queries.h"

static void mbr_of_poly(const struct spatial_range *poly, struct spatial_range *mbr, int n);

/**
 * Returns a list of geohashes in the grid that have data and are contained in the query.
 *
 * @param grid - hashmap of grid cells
 * @param bbox - the query as a GeoCoord (spatial range)
 *
 * @return list of matching grid cells that contain data
 */
char** matching_grid_cells(struct geode *grid, const struct spatial_range *poly, int n) {
  struct spatial_range mbr;
  mbr_of_poly(poly, &mbr, n);

  /* Geodes in the system that bound this search */
  char *north_w = geohash_encode(mbr.north, mbr.west, PREFIX_SZ);
  char *north_e = geohash_encode(mbr.north, mbr.east, PREFIX_SZ);
  char *south_e = geohash_encode(mbr.south, mbr.east, PREFIX_SZ);
  
  /* All geodes within the query */
  char **hashes = calloc(1, sizeof(char *));

  /* Geodes that have data and are also within the query */
  char **match_hashes = calloc(1, sizeof(char *));

  /* Count of all geodes within the query bounds */
  int count = 0;
  /* Count of all geodes that have data and are within the query bounds */
  int match_count = 0;

  /* Use the North Western geode of the query as a starting point */
  struct geode *g;

  /*Save start  of current row*/
  char *start = north_w;
  char *curr = start;

  /* Keep track of spot in row and row size */
  int width = -1;
  int g_width = width;
  
  /* While there are more geodes */
  while (true) {
  
    /* If the geode exists in the system and has data in the bounds of the query we will add it to our match list */
    HASH_FIND_STR(grid, curr, g);
    if (g != NULL) {
      /*Count increases no matter what and we add the geode to our complete list */
      hashes = count != 0 ? realloc(hashes, sizeof(char *) * (count + 1)) : hashes;
      hashes[count++] = curr;
      if (polygon_intersects_geode(g, poly, n, count)) {
        match_hashes = match_count != 0 ? realloc(match_hashes, sizeof(char *) * (match_count + 1)) : match_hashes;
        match_hashes[match_count++] = strdup(curr);
      }
    }
    
    /* If we hit the last geode and have added it then end */
    if (strcmp(curr, south_e) == 0) {
      break;
    }

    /* If we hit top right save width of the rows */
    if ((strcmp(curr, north_e) == 0)) {
      width = count;
      g_width = width;
      curr = get_neighbor(start, SOUTH);
      /* Change start to new row and resume working east */
      start = curr;
    }

    /* If done with row reset row count and toggle south*/
    else if (width == 0) {
      width = g_width;
      curr = get_neighbor(start, SOUTH);
      /* Change start to new row and resume working east */
      start = curr;
    } else {
      curr = get_neighbor(curr, EAST);
    }

    width--;
  }
  
  /* NULL termniate the list of matches */
  match_hashes = realloc(match_hashes, sizeof(char *) * (match_count + 1));
  match_hashes[match_count] = NULL;

  return match_hashes;
}

/**
 * Check if any geohashes in the grid that have data and are contained in the query.
 * 
 * @param grid  - hashmap of grid cells
 * @params bbox - the query as a GeoCoord (spatial range)
 *
 * @return true if any matching grid cells contain data
 */
bool has_matching_grid_cells(struct geode *grid, const struct spatial_range *poly, int n) {
  struct spatial_range mbr;
  mbr_of_poly(poly, &mbr, n);

  /* Geodes in the system that bound this search */
  char *north_w = geohash_encode(mbr.north, mbr.west, PREFIX_SZ);
  char *north_e = geohash_encode(mbr.north, mbr.east, PREFIX_SZ);
  char *south_e = geohash_encode(mbr.south, mbr.east, PREFIX_SZ);
  
  /* Count of all geodes within the query bounds */
  int count = 0;

  /* Use the North Western geode of the query as a starting point */
  struct geode *g;

  /* Save start of current row*/
  char *start = north_w;
  char *curr = start;

  /* Keep track of the spot in row and row size */
  int width = -1;
  int g_width = width;
  
  /* While there are more geodes */
  while (true) {
  
    /* If the geode exists in the system and has data in the bounds of the query it is added to the match list */
    HASH_FIND_STR(grid, curr, g);
    if (g != NULL) {
      /*Count increases no matter what and the geode is added to the complete list */
      count++;
      if (polygon_intersects_geode(g, poly, n, count)) {
        return true;
      }
    }
    
    /* If we hit the last geode and have added it then end */
    if (strcmp(curr, south_e) == 0) {
      break;
    }

    /* If we hit top right save width of the rows */
    if ((strcmp(curr, north_e) == 0)) {
      width = count;
      g_width = width;
      curr = get_neighbor(start, SOUTH);
      /* Change start to new row and resume working east */
      start = curr;
    } else if (width == 0) {
    /* If done with row reset row count and toggle south*/
      width = g_width;
      curr = get_neighbor(start, SOUTH);
      /* Change start to new row and resume working east */
      start = curr;
    } else {
      curr = get_neighbor(curr, EAST);
    }

    width--;
  }

  return false;
}

static void mbr_of_poly(const struct spatial_range *poly, struct spatial_range *mbr, int n) {
    mbr->east = MIN_LONG;
    mbr->west = MAX_LONG;
    mbr->south = MAX_LAT;
    mbr->north = MIN_LAT;

    for (int i = 0; i < n; i++) {
        if (poly[i].longitude < mbr->west) {
            mbr->west = poly[i].longitude;
        }
        if (poly[i].longitude > mbr->east) {
            mbr->east = poly[i].longitude;
        }
        if (poly[i].latitude > mbr->north) {
            mbr->north = poly[i].latitude;
        }
        if (poly[i].latitude < mbr->south) {
            mbr->south = poly[i].latitude;
        }
    }
    printf("MBR North: %f West: %f South: %f East: %f\n", mbr->north, mbr->west, mbr->south, mbr->east);
} 
