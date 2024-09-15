// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "pipe_group_adt.h"
#include "error_handling.h"

#define MESSAGE_SIZE 1024
#define READ_TIMEOUT_NS 100000



typedef struct PipeGroupCDT {
    int size;
    int** read_pipes;
    int** write_pipes;
} PipeGroupCDT;



PipeGroupADT new_pipe_group(int size) {
    PipeGroupADT group = (PipeGroupADT)malloc(sizeof(PipeGroupCDT));
    CHECK_NULL(group);

    group->size = size;
    group->read_pipes = (int**)malloc(size * sizeof(int*));
    CHECK_NULL(group->read_pipes);

    group->write_pipes = (int**)malloc(size * sizeof(int*));
    CHECK_NULL(group->write_pipes);

    for(int i = 0; i < size; i++) {
        group->read_pipes[i] = (int*)malloc(2 * sizeof(int));
        CHECK_NULL(group->read_pipes[i]);
        group->write_pipes[i] = (int*)malloc(2 * sizeof(int));
        CHECK_NULL(group->write_pipes[i]);

        if(pipe(group->write_pipes[i]) == -1 || pipe(group->read_pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

    }
    return group;
}


void select_readable(PipeGroupADT group, int* selected) {
    fd_set read_set;
    FD_ZERO(&read_set);
    int max_fd = 0;
    for(int i = 0; i < group->size; i++) {
        FD_SET(group->read_pipes[i][0], &read_set);
        if(group->read_pipes[i][0] > max_fd) {
            max_fd = group->read_pipes[i][0];
        }
    }

    struct timeval read_timeout = {0, READ_TIMEOUT_NS}; 
    int readable_pipes = select(max_fd + 1, &read_set, NULL, NULL, &read_timeout);
    
    int j = 0;
    for(int i = 0; i < group->size && readable_pipes > 0; i++) {
        if(FD_ISSET(group->read_pipes[i][0], &read_set)) {
            selected[j] = i;
            j++;
            readable_pipes--;
        }
    }
    selected[j] = -1;
}


void choose_pipe_pair(PipeGroupADT group, int i) {
    close(group->write_pipes[i][1]);
    dup2(group->write_pipes[i][0], STDIN_FILENO);
    close(group->write_pipes[i][0]);

    close(group->read_pipes[i][0]);
    dup2(group->read_pipes[i][1], STDOUT_FILENO);
    close(group->read_pipes[i][1]);

    for(int j = 0; j < group->size; j++) {
        if(j != i) {
            close(group->write_pipes[j][0]);
            close(group->write_pipes[j][1]);
        }
    }
}


ssize_t write_pipe_pair(PipeGroupADT group, int i, char* str) {
    char message[MESSAGE_SIZE] = {0};
    int len = snprintf(message, MESSAGE_SIZE, "%s\n", str);
    return write(group->write_pipes[i][1], message, len);
}

ssize_t read_pipe_pair(PipeGroupADT group, int i, char* buffer) {
    return read(group->read_pipes[i][0], buffer, MESSAGE_SIZE);
}


void close_pipes(PipeGroupADT group) {
    char end_of_file[2] = {EOF, 0};

    for(int i = 0; i < group->size; i++) {
        write_pipe_pair(group, i, end_of_file);

        close(group->read_pipes[i][0]);
        close(group->write_pipes[i][1]);

        free(group->read_pipes[i]);
        free(group->write_pipes[i]);
    }

    free(group->read_pipes);
    free(group->write_pipes);

    free(group);
}