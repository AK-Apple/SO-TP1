#ifndef RESULT_ADT_H
#define RESULT_ADT_H

#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define ERROR_EXIT(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct ResultCDT *ResultADT;


ResultADT new_result_ADT(pid_t pid, int file_count);

ResultADT open_result_ADT(int pid, int file_count);

void close_result_ADT(ResultADT Result_ADT);

void free_result_ADT(ResultADT result_ADT);

int print_result(ResultADT result_ADT);

void write_result(ResultADT result_ADT, char *result);

#endif