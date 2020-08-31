#include <stdlib.h>
#include "geode.h"
#include "gd.h"


/**
 * Returns a list of geohashes in the grid that have data and are contained in the query.
 *
 * @param grid - hashmap of grid cells
 * @param bbox - the query as a GeoCoord (spatial range)
 *
 * @return list of matching grid cells that contain data
 */
char** matching_grid_cells(struct geode *grid, GeoCoord *bbox) {

  /* Geodes in the system that bound this search */
  char *north_w = geohash_encode(bbox->north, bbox->west, PREFIX_SZ);
  char *north_e = geohash_encode(bbox->north, bbox->east, PREFIX_SZ);
  char *south_e = geohash_encode(bbox->south, bbox->east, PREFIX_SZ);
  
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
      if (geode_query(g, bbox, count)) {
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
bool has_matching_grid_cells(struct geode *grid, GeoCoord *bbox) {
  /* Geodes in the system that bound this search */
  char *north_w = geohash_encode(bbox->north, bbox->west, PREFIX_SZ);
  char *north_e = geohash_encode(bbox->north, bbox->east, PREFIX_SZ);
  char *south_e = geohash_encode(bbox->south, bbox->east, PREFIX_SZ);
  
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
      if (geode_query(g, bbox, count)) {
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

void matching_grid_cells_polygon(struct geode *grid)
{
    /* Iterate over grid and just query each node for now??? */
    geodePointPtr points = (geodePointPtr)calloc(4, sizeof(gdPoint));
	points[0].x = 0;
	points[0].y = 0;
	points[1].x = 500;
	points[1].y = 500;
	points[2].x = 800;
	points[2].y = 100;

    geode_polygon_query(grid, points, 3);
}
