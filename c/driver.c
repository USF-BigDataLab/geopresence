#include "./bitmap.h"
#include "./common.h"
#include "./geogrid.h"
#include "./geohash.h"
#include "./roaring.h"
#include <sys/time.h>


void print_results(const int iterationSize, const int cardinality,
  double total_insert_time, double total_base_time){

  printf("\nRESULTS\n");
  printf("Total iterations: %d\n", iterationSize);
  printf("Cardinality (per iteration): %d\n", cardinality);
  printf("Total insert time (sec): %f\n", total_insert_time);
  printf("Average insertion time (sec): %f\n", total_insert_time / iterationSize);
  printf("Average base time (sec): %f\n", total_base_time / iterationSize);
  printf("Estimated insertions per second: %f\n", cardinality / total_insert_time);
}

/* Function: trimmed_data_set
 * Creates a new file trimmed to the dataSize limit or all of original file's data
 * Input:
 *    - filename     : File to copy data from
 *    - dataSize     : Limit of data copied
 *    - new_filename : Where the new file is created
 * Returns: Size of new file
*/
int trimmed_data_set(const char *filename, const int dataSize, char *new_filename){
  // 1. Create or override new file w name trimmed_ + filename
  // 2. Copy data over from filename to new file
  //     a. Break when >= dataSize OR when original file reaches end of file
  // 3. Return number of data copied

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

void insertion_benchmark(const char *filename, const int iterationSize) {
    printf("\nSTARTING INSERTION BENCHMARK\n");
    struct timeval start, end;
    double total_insert_time = 0, total_base_time = 0;
    struct rbitmap* test;

    for(int iterations = 0; iterations < iterationSize; iterations++){
      test = init_rbitmap();

      /* Time insertion */
      gettimeofday(&start, NULL);
      rbitmap_add_all(test, filename, 12);
      gettimeofday(&end, NULL);

      /* Get elapsed time for inserting into empty bitmap */
      total_insert_time += get_elapsed_sec(start, end);

      /* Getting time for already populated bitmap,
         no insertions should be happening */
      gettimeofday(&start, NULL);
      rbitmap_add_all(test, filename, 12);
      gettimeofday(&end, NULL);

      total_base_time += get_elapsed_sec(start, end);

      roaring_bitmap_free(test->rbp); //rest for next iteration
    }

    /* Get cardinality */
    test = init_rbitmap();
    rbitmap_add_all(test, filename, 12);
    const int cardinality = roaring_bitmap_get_cardinality(test->rbp);

    print_results(iterationSize, cardinality, total_insert_time, total_base_time);
}

/*
  Function: void buff_insertion_benchmark()
*/
void buff_insertion_benchmark() {
    printf("\nSTARTING BUFF INSERTION BENCHMARK\n");
    const int iterationSize = 5;

    struct timeval start, end;
    double total_insert_time = 0, total_base_time = 0;

    struct rbitmap* test;
    const char* filename = "geohashes.txt";

    for(int iterations = 0; iterations < iterationSize; iterations++){
      test = init_rbitmap();

      /* Time insertion */
      gettimeofday(&start, NULL);
      rbitmap_add_all_buff(test, filename, 12);
      gettimeofday(&end, NULL);

      /* Get elapsed time for inserting into empty bitmap */
      total_insert_time += get_elapsed_sec(start, end);

      /* Getting time for already populated bitmap,
         no insertions should be happening */
      gettimeofday(&start, NULL);
      rbitmap_add_all_buff(test, filename, 12);
      gettimeofday(&end, NULL);

      total_base_time += get_elapsed_sec(start, end);

      roaring_bitmap_free(test->rbp); //reset for next iteration
    }

    /* Get cardinality */
    test = init_rbitmap();
    rbitmap_add_all_buff(test, filename, 12);
    const int cardinality = roaring_bitmap_get_cardinality(test->rbp);

    print_results(iterationSize, cardinality, total_insert_time, total_base_time);
}

int main() {

  const char *geohashFile = "geohashes.txt";
  const int dataSize = 262792; iterationSize = 5;
  char newGeohashFile[125];

  const int dataSetNum = trimmed_data_set(geohashFile, dataSize, newGeohashFile);
  if(dataSetNum < dataSize){
    printf("ERROR: File %s with total set of %d is unabled to be trimmed to %d\n", geohashFile, dataSetNum, dataSize);
    return 1;
  }

  // buff_insertion_benchmark();
  insertion_benchmark(geohashFile, iterationSize);

  remove(newGeohashFile);

  return 0;
}
