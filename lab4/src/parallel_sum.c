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
#include "utils.h"
#include "par_sum.h"
#include <pthread.h>

int main(int argc, char **argv) {
// Прием аргументов командной строки --threads_num, --seed, --array_size
int num_threads = -1;
int random_seed = -1;
int arr_size = -1;

while (true) {
int current_optind = optind ? optind : 1;

static struct option cmd_options[] = {{"threads_num", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

int option_index = 0;
int c = getopt_long(argc, argv, "f", cmd_options, &option_index);

if (c == -1) break;

switch (c) 
{
  case 0:
  switch (option_index) 
  {
    case 0:
    num_threads = atoi(optarg);
    if (num_threads <= 0)
    { 
      printf("Invalid threads number\n");
      return 1;
    }
    break;
    case 1:
    random_seed = atoi(optarg);

    if (random_seed <= 0)
    { 
      printf("Invalid seed value\n");
      return 1;
    }
    break;
    case 2:
    arr_size = atoi(optarg);
    if (arr_size <= 0) 
    {
      printf("Invalid array size\n");
      return 1;
    }
    break;

    default:
    printf("Option index %d is not supported\n", option_index);
  }
  break;

  case '?':
  break;

  default:
  printf("Unexpected character code from getopt: 0%o\n", c);
}
}

if (optind < argc) 
{
  printf("Additional arguments detected but not supported\n");
  return 1;
}

if (random_seed == -1 || arr_size == -1 || num_threads == -1) 
{
  printf("Correct usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\"\n",
  argv[0]);
  return 1;
}

int final_result = 0;

// Создание потоков с использованием POSIX threads
pthread_t worker_threads[num_threads];

// Генерация массива с помощью GenerateArray (время генерации не замеряется)
int *data_array = malloc(sizeof(int) * arr_size);
GenerateArray(data_array, arr_size, random_seed);

// Замер времени выполнения только параллельного подсчета суммы
struct timeval time_start, time_end;
gettimeofday(&time_start, NULL);

// Параллельное вычисление суммы массива
int segment_length = arr_size / num_threads;
struct SumArgs thread_arguments[num_threads];
for (int i = 0; i < num_threads; i++) 
{
  thread_arguments[i].array = data_array;
  thread_arguments[i].begin = i * segment_length;
  thread_arguments[i].end = (i == num_threads - 1) ? arr_size : (i + 1) * segment_length;
  if (pthread_create(&worker_threads[i], NULL, ThreadSum, (void *)&thread_arguments[i])) 
  {
    printf("Failed to create worker thread\n");
    return 1;
  }
}

// Ожидание завершения потоков и сбор результатов
final_result = 0;
for (int i = 0; i < num_threads; i++) 
{
  void *thread_output;
  pthread_join(worker_threads[i], &thread_output);
  final_result += (int)(size_t)thread_output;
}

// Вывод времени выполнения и результата
gettimeofday(&time_end, NULL);
double time_elapsed = (time_end.tv_sec - time_start.tv_sec) * 1000.0;
time_elapsed += (time_end.tv_usec - time_start.tv_usec) / 1000.0;
printf("Execution time: %fms\n", time_elapsed);

free(data_array);
printf("Computed total: %d\n", final_result);
return 0;
}