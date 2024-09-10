#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "PipeGroupADT.h"

#define BUF_SIZE 1024

typedef struct PipeGroupCDT{
    int size;
    int** read_pipes;
    int** write_pipes;
} PipeGroupCDT;


// creates "size" pipe pairs (for reading and writing) and returns a pointer to the group
PipeGroupADT new_pipe_group(int size){
    PipeGroupADT group = (PipeGroupADT)malloc(sizeof(PipeGroupCDT));
    group->size = size;
    group->read_pipes = (int**)malloc(size * sizeof(int*));
    group->write_pipes = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        group->read_pipes[i] = (int*)malloc(2 * sizeof(int));
        group->write_pipes[i] = (int*)malloc(2 * sizeof(int));

        if (pipe(group->write_pipes[i]) == -1 || pipe(group->read_pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

    }

    // printf("pipes creados\n");
    return group;
}

int* select_readable(PipeGroupADT group){

    // 1. Create set of read pipes
    fd_set read_set;
    FD_ZERO(&read_set);
    int max_fd = 0;
    for(int i=0; i<group->size; i++){
        FD_SET(group->read_pipes[i][0], &read_set);
        if (group->read_pipes[i][0] > max_fd) {
            max_fd = group->read_pipes[i][0];
        }
    }

    // 2. select readable pipes
    struct timeval read_timeout = {0, 100000}; // 0.1 second timeout
    int readable_pipes = select(max_fd + 1, &read_set, NULL, NULL, &read_timeout);
    // printf("%d\n", readable_pipes);

    // 3. Return
    int* selected = (int*)malloc((group->size+1) * sizeof(int));
    int i;
    int j=0;
    for(i=0; i<group->size && readable_pipes > 0; i++){
        if(FD_ISSET(group->read_pipes[i][0], &read_set)){
            selected[j]=i;
            j++;
            readable_pipes--;
        }
    }
    selected[j]=-1;
    return selected;
}

// chooses the i-th pipe pair and manipulates it so the program can read from the "read" pipe and write to the "write" pipe and closes anything else
void choose_pipe_pair(PipeGroupADT group, int i){

    // 1. manipulation of i-th pipe
    close(group->write_pipes[i][1]); // borro canal "write end"
    dup2(group->write_pipes[i][0], STDIN_FILENO); // asigno fd=0 a "read end"
    close(group->write_pipes[i][0]); // cierro fd extra"

    close(group->read_pipes[i][0]); // borro canal "read end"
    dup2(group->read_pipes[i][1], STDOUT_FILENO); // asigno fd=1 a "write end"
    close(group->read_pipes[i][1]); // cierro fd extra"

    // 2. time to close everything else
    for(int j=0; j<group->size; j++){
        if (j!=i) {
            close(group->write_pipes[j][0]);
            close(group->write_pipes[j][1]);
        }
    }
}


ssize_t write_pipe_pair(PipeGroupADT group, int i,char* str){
    return write(group->write_pipes[i][1], str, strlen(str));
}

ssize_t read_pipe_pair(PipeGroupADT group, int i, char* buffer){
    return read(group->read_pipes[i][0], buffer, BUF_SIZE);
}

// sends EOF to all the pipes and closes them
void close_pipes(PipeGroupADT group){
    char eof = -1;
    for (int i = 0; i < group->size; i++) {
        write_pipe_pair(group, i, &eof);
        close(group->read_pipes[i][0]);
        close(group->write_pipes[i][1]);
    }
    // printf("pipes cerrados\n");
    // TODO: agregar FREE TODITO
}