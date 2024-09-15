// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include "pipe_group_adt.h"
#include "result_adt.h"
#include "error_handling.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SLAVES_PER_100_FILES 5
#define MAX_FILES_PER_SLAVE 2
#define MAX_SLAVES_COUNT 20
#define MESSAGE_MAX_SIZE 256
#define RESULT_BUFFER_SIZE 4096
#define VIEW_SLEEP 6
#define SLAVE_NAME "slave"

static int instances_of_char(char* str, char c);

static int ceil_of_fraction(int x, int y);

static void initial_distribution(int files_per_slave, int slaves_count, PipeGroupADT pipe_group, int local_to_read[], char* argv[]);

static void main_loop(int files_per_slave, int slaves_count, int files_amount, PipeGroupADT pipe_group, ResultADT result_adt, int local_to_read[], char* argv[]);

int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    int files_amount = argc - 1;
    if (files_amount == 0) return 0;
    int slaves_count = MIN(ceil_of_fraction(files_amount * SLAVES_PER_100_FILES, 100), MAX_SLAVES_COUNT);
    int files_per_slave = MIN(files_amount / slaves_count, MAX_FILES_PER_SLAVE);

    pid_t pid = getpid();
    ResultADT result_adt = new_result_adt(pid, files_amount);
    printf("%d %d\n", pid, files_amount);
    sleep(VIEW_SLEEP);

    PipeGroupADT pipe_group = new_pipe_group(slaves_count);

    pid_t children_pid[MAX_SLAVES_COUNT] = {0};
    for (int i = 0; i < slaves_count; i++) {
        char* const param_list[2] = {SLAVE_NAME, NULL};
        pid_t child_pid = fork();
        if (child_pid == -1) {
            ERROR_EXIT("fork");
        }
        else if (child_pid == 0) {
            choose_pipe_pair(pipe_group, i);

            int exec_slave_status = execve(SLAVE_NAME, param_list, 0);
            if(exec_slave_status == -1) { 
                ERROR_EXIT("execve");
            }
        }
        else {
            children_pid[i] = child_pid;
        }
    }

    int local_to_read[MAX_SLAVES_COUNT] = {0};
    initial_distribution(files_per_slave, slaves_count, pipe_group, local_to_read, argv);
    main_loop(files_per_slave, slaves_count, files_amount, pipe_group, result_adt, local_to_read, argv);

    close_pipes(pipe_group);
    for (int i = 0; i < slaves_count; i++) {
        waitpid(children_pid[i], NULL, 0);
    }
    free_result_adt(result_adt);

    return 0;
}


static int instances_of_char(char* str, char c) {
    int i, count;
    for (i = 0, count = 0; str[i]; i++)
        count += (str[i] == c);
    return count;
}

static int ceil_of_fraction(int x, int y) {
    return (x + y - 1) / y;
}

static void main_loop(int files_per_slave, int slaves_count, int files_amount, PipeGroupADT pipe_group, ResultADT result_adt, int local_to_read[], char* argv[]) {
    int read_files = 0;
    int written_files = slaves_count * files_per_slave;
    int* selected = (int*)malloc((slaves_count + 1) * sizeof(int));
    CHECK_NULL(selected);

    while (read_files < files_amount) {
        select_readable(pipe_group, selected);

        char result_buffer[RESULT_BUFFER_SIZE] = {0};

        for (int j = 0; selected[j] != -1 && read_files < files_amount; j++) {
            int i = selected[j];

            ssize_t bytes_read = read_pipe_pair(pipe_group, i, result_buffer);
            result_buffer[bytes_read] = '\0';

            write_result_adt(result_adt, result_buffer);

            int files_this_iteration = instances_of_char(result_buffer, '\n');
            read_files += files_this_iteration;
            local_to_read[i] -= files_this_iteration;

            if (local_to_read[i] == 0 && written_files < files_amount) {
                written_files++;
                local_to_read[i]++;
                write_pipe_pair(pipe_group, i, argv[written_files]);
            }
        }
    }

    free(selected);
}

static void initial_distribution(int files_per_slave, int slaves_count, PipeGroupADT pipe_group, int local_to_read[], char* argv[]) {
    for (int i = 0; i < slaves_count; i++) {
        for (int j = 0; j < files_per_slave; j++) {
            int written_files = i * files_per_slave + j + 1;
            write_pipe_pair(pipe_group, i, argv[written_files]);
        }
        local_to_read[i] = files_per_slave;
    }
}