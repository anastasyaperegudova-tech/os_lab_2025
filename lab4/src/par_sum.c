#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include "utils.h"
#include <pthread.h>

struct SumArgs {
int *array;
int begin;
int end;
};

int Sum(const struct SumArgs *args) {
int accumulator = 0;
for (int index = args->begin; index < args->end; index++) 
accumulator += args->array[index];
return accumulator;
}

void *ThreadSum(void *args) {
struct SumArgs *computation_args = (struct SumArgs *)args;
return (void *)(size_t)Sum(computation_args);
}