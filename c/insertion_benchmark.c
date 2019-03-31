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

double get_elapsed_sec(struct timeval start, struct timeval end){
  double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0; // sec to ms
  elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0; // us to ms
  elapsed_time = elapsed_time / 1000.0; // convert to sec

  return elapsed_time;
}

void insertion_benchmark() {
    printf("\nSTARTING INSERTION BENCHMARK\n");
    const int iterationSize = 5;

    struct timeval start, end;
    double elapsed_time, total_insert_time = 0, total_base_time = 0;

    struct rbitmap* test;
    const char* filename = "geohashes.txt";

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
  Input: None
  Output: None
  Description: This function is used specifcally for benchmarking; this will
  allow us to see the time performance of inserting points into the roaring
  bitmap.
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

      roaring_bitmap_free(test->rbp); //rest for next iteration
    }

    /* Get cardinality */
    test = init_rbitmap();
    rbitmap_add_all_buff(test, filename, 12);
    const int cardinality = roaring_bitmap_get_cardinality(test->rbp);

    print_results(iterationSize, cardinality, total_insert_time, total_base_time);
}

int main() {

  buff_insertion_benchmark();

  insertion_benchmark();

  return 0;
}
