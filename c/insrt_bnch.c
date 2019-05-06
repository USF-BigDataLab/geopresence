#include "./driver.h"
#include <sys/time.h>

#define PRECISION 3      // base geo coord precision
#define ITERATION_SZ 100 // how many times to run insertion loop per data size
#define DATA_SZ 10       // number of data sizes to run

/* Global */
// Each data size is the number of hashes being read
int dataSizes [DATA_SZ] = {26000, 52000, 78000, 104000, 130000, 156000, 182000, 208000, 234000, 260000};
const char *geohashFile = "geohashes.txt"; // has 262,792 unique hashes
// const int dataSize = 26000;

void print_results(int dataSize, double avg_insert_time){
  printf("\nRESULTS\n");
  printf("Total iterations: %d\n", ITERATION_SZ);
  printf("Total inserts (per iteration): %d\n", dataSize);
  // printf("Total insert time (sec): %f\n", total_insert_time);
  // printf("Average insertion time (sec): %f\n", avg_insert_time);
  printf("Average insertion time (ms): %f\n", avg_insert_time);
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

/*
* Function: just_read_file
* Reads the file given. Used to get a base time for test.
* Input:
*     - file_path: File to read
* Returns: Count of lines read
*/
int just_read_file(const char *file_path){
  FILE *fp;
  const int buff_sz = 255;
  char buff[buff_sz];
  int count = 0;

  fp = fopen(file_path, "r");
  while(fgets(buff, buff_sz, (FILE*) fp)){
    count += 1;
  }

  fclose(fp);
  return count;
}

void insertion_benchmark(const char *filename, double *avg_insert_time) {
    // printf("\nReading file: %s\n", filename);
    // TODO: Get the avg insert time of all iterations
    double running_insert_time = 0;
    struct timeval start, end;
    double total_base_time = 0;

    for(int iter = 0; iter < ITERATION_SZ; iter++){

      /* Time insertion */
      gettimeofday(&start, NULL);
      read_file(filename, PRECISION);
      gettimeofday(&end, NULL);

      /* Get elapsed time for inserting into empty bitmap */
      // running_insert_time += get_elapsed_sec(start, end);
      running_insert_time += get_elapsed_ms(start, end);

      /* Getting base time, just time to read file. No insertions. */
      gettimeofday(&start, NULL);
      just_read_file(filename);
      gettimeofday(&end, NULL);

      // total_base_time += get_elapsed_sec(start, end);
      total_base_time += get_elapsed_ms(start, end);
      free_hm(); // reset
    }

    *avg_insert_time = running_insert_time / ITERATION_SZ;
}

int main() {

  printf("\nSTARTING INSERTION BENCHMARK\n");
  FILE *r_fp = fopen("insertion_benchmark_results.txt", "w");
  double avg_insert_time;
  for(int i = 0; i < DATA_SZ; i++){
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

  return 0;
}
