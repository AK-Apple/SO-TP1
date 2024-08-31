#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>


#define BUF_SIZE 1024   /* Maximum size for exchanged string */
#define STR_SIZE 128
#define SHM_NAME_MAX 256

typedef struct {
    char name[STR_SIZE];
    char md5[STR_SIZE];
    int id;
} Info;

typedef struct {
    sem_t  sem1;            /* POSIX unnamed semaphore */
    sem_t  sem2;            /* POSIX unnamed semaphore */
    size_t cnt;             /* Number of bytes used in 'buf' */
    Info   buf[BUF_SIZE];   /* Data being transferred */
} SharedInfo;


#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)