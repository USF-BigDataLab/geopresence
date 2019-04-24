#include "./bitmap.h"
#include "./common.h"
#include "./geogrid.h"
#include "./geohash.h"
#include "./roaring.h"
#include <sys/time.h>

#define PRECISION 12
#define ITERATION_SZ 100
#define DATA_SZ 10

// Global
int dataSizes [DATA_SZ] = {26000, 52000, 78000, 104000, 130000, 156000, 182000, 208000, 234000, 260000};
const int dataSize = 26000;
const char *geohashFile = "geohashes.txt"; // has 262,792 hashes

void print_results(int dataSize, double avg_insert_time){
  printf("\nRESULTS\n");
  printf("Total iterations: %d\n", ITERATION_SZ);
  printf("Total inserts (per iteration): %d\n", dataSize);
  // printf("Total insert time (sec): %f\n", total_insert_time);
  printf("Average insertion time (sec): %f\n", avg_insert_time);
  // printf("Average insertion time (ms): %f\n", avg_insert_time);
  // printf("Average insertions per second: %f\n", dataSize / total_insert_time);
}

/* Function: trim_data_set
 * Creates a new file trimmed to the dataSize limit or all of original file's data
 * Input:
 *    - filename     : File to copy data from
 *    - dataSize     : Limit of data copied
 *    - new_filename : Where the new file is created
 * Returns: Size of new file
*/
int trim_data_set(const char *filename, const int dataSize, char *new_filename){
  sprintf(new_filename, "trimmed_%s", filename);

  FILE *fp, *nfp;
  char buff[255];
  int dataCount = 0;

  fp = fopen(filename, "r");
  nfp = fopen(new_filename, "w");
  while(fgets(buff, 255, (FILE*) fp) && dataCount < dataSize){
    fputs(buff, nfp);
    dataCount += 1;
  }
  fclose(fp);
  fclose(nfp);

  return dataCount;
}

double get_elapsed_sec(struct timeval start, struct timeval end){
  double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0; // sec to ms
  elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0; // us to ms
  elapsed_time = elapsed_time / 1000.0; // convert to sec

  return elapsed_time;
}

double get_elapsed_ms(struct timeval start, struct timeval end){
  return get_elapsed_sec(start, end) * 1000;
}

void insertion_benchmark(const char *filename, double *avg_insert_time) {
    // printf("\nReading file: %s\n", filename);
    // TODO: Get the avg insert time of all iterations
    double running_insert_time = 0;
    struct timeval start, end;
    double total_base_time = 0;
    struct rbitmap* test;

    for(int iter = 0; iter < ITERATION_SZ; iter++){
      test = init_rbitmap();

      /* Time insertion */
      gettimeofday(&start, NULL);
      rbitmap_add_all(test, filename, PRECISION);
      gettimeofday(&end, NULL);

      /* Get elapsed time for inserting into empty bitmap */
      running_insert_time += get_elapsed_sec(start, end);
      // running_insert_time += get_elapsed_ms(start, end);

      /* Getting time for already populated bitmap,
         no insertions should be happening */
      gettimeofday(&start, NULL);
      rbitmap_add_all(test, filename, PRECISION);
      gettimeofday(&end, NULL);

      total_base_time += get_elapsed_sec(start, end);
      // total_base_time += get_elapsed_ms(start, end);

      roaring_bitmap_free(test->rbp); //rest for next iteration
    }

    *avg_insert_time = running_insert_time / ITERATION_SZ;

    /* Get cardinality */
    test = init_rbitmap();
    rbitmap_add_all(test, filename, PRECISION);
    const int cardinality = roaring_bitmap_get_cardinality(test->rbp);
    printf("Cardinality: %d\n", cardinality);
}

/*
  Function: void buff_insertion_benchmark()
*/
void buff_insertion_benchmark() {
    printf("\nSTARTING BUFF INSERTION BENCHMARK\n");

    struct timeval start, end;
    double total_insert_time = 0, total_base_time = 0;

    struct rbitmap* test;
    const char* filename = "geohashes.txt";

    for(int iterations = 0; iterations < ITERATION_SZ; iterations++){
      test = init_rbitmap();

      /* Time insertion */
      gettimeofday(&start, NULL);
      rbitmap_add_all_buff(test, filename, PRECISION);
      gettimeofday(&end, NULL);

      /* Get elapsed time for inserting into empty bitmap */
      total_insert_time += get_elapsed_sec(start, end);

      /* Getting time for already populated bitmap,
         no insertions should be happening */
      gettimeofday(&start, NULL);
      rbitmap_add_all_buff(test, filename, PRECISION);
      gettimeofday(&end, NULL);

      total_base_time += get_elapsed_sec(start, end);

      roaring_bitmap_free(test->rbp); //reset for next iteration
    }

    /* Get cardinality */
    // test = init_rbitmap();
    // rbitmap_add_all_buff(test, filename, PRECISION);
    // const int cardinality = roaring_bitmap_get_cardinality(test->rbp);

    // print_results(ITERATION_SZ, cardinality, total_insert_time, total_base_time);
}

void hashmap_test() {
  char* test = malloc(sizeof(char) * 100);
  test = "9x";
  add_cell(test, 12);

  struct bitmap_hm_data *result = find_cell(test);
  printf("Printing the value retrieved from the hashmap: %d\n", result->cardinality);
  //delete_cell(test);
  unsigned int len = get_hm_length();
  printf("Printing the length of the hashmap: %u\n", len);
  print_cells();
}

int main() {
  printf("\nSTARTING INSERTION BENCHMARK\n");
  FILE *r_fp = fopen("results.txt", "w");
  double avg_insert_time;
  for(int i = 0; i < 1; i++){
    avg_insert_time = 0;
    char newGeohashFile[125];

    const int dataSetNum = trim_data_set(geohashFile, dataSizes[i], newGeohashFile);
    if(dataSetNum < dataSizes[i]){
      printf("ERROR: File %s with total set of %d is unabled to be trimmed to %d\n", newGeohashFile, dataSetNum, dataSizes[i]);
      return 1;
    }

    insertion_benchmark(newGeohashFile, &avg_insert_time);
    print_results(dataSizes[i], avg_insert_time);
    fprintf(r_fp, "%d\t%f\n", dataSizes[i], avg_insert_time);

    remove(newGeohashFile);
  }
  fclose(r_fp);
  

  // struct rbitmap* test = init_rbitmap();
  // rbitmap_add_all(test, geohashFile, PRECISION);
  // buff_insertion_benchmark();
  // hashmap_test();

  return 0;
}
