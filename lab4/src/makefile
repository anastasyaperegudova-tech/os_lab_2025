CC=gcc
CFLAGS=-I.

all: p_m_m process_memory parallel_sum

parallel_sum: parallel_sum.o utils.o libparsum.a
	$(CC) parallel_sum.o utils.o -L. -lparsum -o parallel_sum

libsum_p.a: parallel_s.o
	ar rcs libparsum.a par_sum.o

parallel_s.o:
	$(CC) -lpthread par_sum.c -o parallel_s.o

parallel_sum.o : utils.o
	$(CC) -o parallel_sum.o -c parallel_sum.c utils.o $(CFLAGS)

process_memory: 
	$(CC) -o process_memory process_memory.c $(CFLAGS)

p_m_m: utils.o find_min_max.o
	$(CC) -o p_m_m utils.o find_min_max.o p_m_m.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o p_m_m process_memory