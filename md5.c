// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#include "PipeGroupADT.h"
#include "ResultADT.h"
#include "error_handling.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SLAVES_PER_100_FILES 5
#define MAX_FILES_PER_SLAVE 2
#define MAX_SLAVES_COUNT 20
#define MESSAGE_MAX_SIZE 128


#define VIEW_SLEEP 2

int instances_of_char(char* str, char c);

int ceil_of_fraction(int x, int y);


int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0); 

    int files_amount = argc - 1;
    int slaves_count = MIN(ceil_of_fraction(files_amount * SLAVES_PER_100_FILES, 100), MAX_SLAVES_COUNT);
    int files_per_slave = MIN(files_amount / slaves_count, MAX_FILES_PER_SLAVE);
    
    pid_t pid = getpid();
    ResultADT result_ADT = new_result_ADT(pid, files_amount);
    printf("%d %d\n", pid, files_amount);
    sleep(VIEW_SLEEP);

    PipeGroupADT pipe_group = new_pipe_group(slaves_count);

    pid_t children_pid[MAX_SLAVES_COUNT] = {0}; 
    for(int i = 0; i < slaves_count; i++) { 
        char* slave_name = "slave";    
        char* const param_list[2] = {slave_name, NULL};
        pid_t child_pid = fork();
        if(child_pid == -1) {
            perror("fork");
            return 1;
        }
        else if(child_pid == 0) {
            choose_pipe_pair(pipe_group, i);

            int ret = execve(slave_name, param_list, 0);
            if(-1 == ret){
                perror("execve");
                return -1;
            }
        }
        else {
            children_pid[i] = child_pid;
        }
    }

    char message[MESSAGE_MAX_SIZE];
    int local_to_read[MAX_SLAVES_COUNT] = {0};

    for(int i=0; i<slaves_count; i++){
        for(int j=0; j<files_per_slave; j++){
            int written_files = i*files_per_slave + j;
            snprintf(message, MESSAGE_MAX_SIZE, "%s\n", argv[written_files + 1]);
            write_pipe_pair(pipe_group, i, message);
        }
        local_to_read[i] = files_per_slave;
    }

    int read_files = 0;
    int written_files = slaves_count * files_per_slave;


// ----------------------------  Start Main Loop -------------------------------

    int* selected = (int*)malloc(slaves_count * sizeof(int));
    CHECK_NULL(selected);

    while(read_files < argc-1) {
        select_readable(pipe_group, selected);
        
        char buffer[4096];

        for(int j = 0; selected[j] != -1 && read_files < argc - 1; j++){
            int i = selected[j];
            
            ssize_t bytes_read = read_pipe_pair(pipe_group, i, buffer);
            buffer[bytes_read] = '\0';

            write_result(result_ADT, buffer);

            int files_this_iteration = instances_of_char(buffer, '\n');
            read_files += files_this_iteration;
            local_to_read[i] -= files_this_iteration;

            if (local_to_read[i] == 0 && written_files < argc - 1){
                snprintf(message, MESSAGE_MAX_SIZE, "%s\n", argv[written_files+1]);
                write_pipe_pair(pipe_group, i, message);
                written_files++;
                local_to_read[i]++;
            }
        }   
    }

// ----------------------------  End Main Loop -------------------------------
   
    close_pipes(pipe_group);
    free(selected);

    for(int i = 0; i < slaves_count; i++) {
        waitpid(children_pid[i], NULL, 0);
    }

    free_result_ADT(result_ADT);

    return 0;
}


int instances_of_char(char* str, char c) {
    int i, count;
    for (i = 0, count = 0; str[i]; i++)
        count += (str[i] == c);
    return(count);
}

int ceil_of_fraction(int x, int y) {
    return (x + y - 1) / y;
}
