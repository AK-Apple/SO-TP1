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

typedef struct ResultCDT* ResultADT;


ResultADT new_result_adt(pid_t pid, int file_count);

ResultADT open_result_adt(int pid, int file_count);

void close_result_adt(ResultADT result_adt);

void free_result_adt(ResultADT result_adt);

int print_result_adt(ResultADT result_adt);

void write_result_adt(ResultADT result_adt, char* result);

#endif