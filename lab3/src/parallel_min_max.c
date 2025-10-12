
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
    // Далее просто проверяем что входные данные больще 0

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
             if (seed <= 0 || seed == NULL){ 
                printf("Error seed\n");
                return 1;}
          
            break;
          case 1:
            array_size = atoi(optarg);

             if (array_size <= 0 || array_size == NULL){ 
                printf("Error array_size\n");
                return 1;}
         
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0 || pnum == NULL) {
                printf("Error pnum\n");
                return 1;
            }
      
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1  || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;


  // Создадим объект pipe
  int p[2], pid, nbytes;

  if (pipe(p) < 0)
        exit(1);

  // Длина участка, обрабатываемая каждым потоком
  int chunk = array_size / pnum + (array_size % pnum ? 1: 0);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
    
      active_child_processes += 1;


      if (child_pid == 0) {

        // начало обработки
        int start = i * chunk;
        int end = ((i == pnum - 1) ? array_size: start + chunk);


        struct MinMax TrMinMax = GetMinMax(array, start, end);
        if (with_files) {
          char filename[32];
          sprintf(filename, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "w");
          if (file == "NULL")
          {
            printf("Error file\n");
            exit(1);

          }
          fprintf(file, "%d %d", TrMinMax.min, TrMinMax.max);
          fclose(file);
        } else {
          //Запись в pipe
          write(p[1], &TrMinMax, sizeof(struct MinMax));

        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Ожидаем завершения всех дочерних процессов
  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

if (with_files) {
        // Чтение из file
          char filename[32];
          sprintf(filename, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "r");
          if (file == "NULL")
          {
            printf("Error file\n");
        

          }
          fscanf(file, "%d %d", &min, &max);
          fclose(file);
          remove(filename);
    } else {
        struct MinMax localMM;
        // Чтение из pipea
        read(p[0], &localMM, sizeof(struct MinMax));
        min = localMM.min;
        max = localMM.max; 
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
