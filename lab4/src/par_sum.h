#ifndef PARALLEL_S_H
#define PARALLEL_S_H

// >>>>> ОПРЕДЕЛЕНИЕ СТРУКТУР: Интерфейс библиотеки <<<<<
struct SumArgs {
    int *array;
    int begin;
    int end;
};

int Sum(const struct SumArgs *args);
void *ThreadSum(void *args);

#endif