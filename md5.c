/* Programa: md5.c aplication */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define SLAVE_COUNT 5


void manipulate_pipes(int write_pipefd[SLAVE_COUNT][2], int read_pipefd[SLAVE_COUNT][2], int i);


int main(int argc, char *argv[]) {
    printf("Esto es md5.c: argcount=%d\n", argc);

    // "Debe iniciar los procesos esclavos."


    // Creo arrays de Pipes

    int write_pipefd[SLAVE_COUNT][2];
    int read_pipefd[SLAVE_COUNT][2];

    for(int i=0; i<SLAVE_COUNT; i++){
        if (pipe(write_pipefd[i]) == -1 || pipe(read_pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }  
    }

    // Hago forks

    pid_t children_pid[SLAVE_COUNT] = {0}; 
    for(int i = 0; i < SLAVE_COUNT; i++) { 
        const char *slave_name = "slave";
        char * const param_list[2] = {slave_name, NULL};
        pid_t child_pid = fork();
        if(child_pid == -1) {
            perror("fork");
            return 1;
        }
        else if(child_pid == 0) {

            // 3. Manipulo pipes
            manipulate_pipes(write_pipefd, read_pipefd, i);

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

    // Escribo a esclavos
    //    canal:    write_pipefd[i][1]

    char message[100];
    for(int i=0; i < SLAVE_COUNT; i++){
        // sprintf(message, "Mensaje numero %d\n", i);
        sprintf(message, "vista.c");

        write(write_pipefd[i][1], message, strlen(message));
    }

    // Leo de esclavos
    //    canal:    read_pipefd[i][0]

    char buffer[100];
    for(int i=0; i < SLAVE_COUNT; i++){
        
        read(read_pipefd[i][0], buffer, sizeof(buffer));
        printf(buffer);
    }



    // 6. termina todo

    for(int i=0; i<SLAVE_COUNT; i++){
        wait(NULL);
    }
    puts("md5 application terminated successfully");
    printf("random child_pid to avoid warning: %d\n", children_pid[0]);

    return 0;
}



void manipulate_pipes(int write_pipefd[SLAVE_COUNT][2], int read_pipefd[SLAVE_COUNT][2], int i){
    close(write_pipefd[i][1]); // borro canal "write end"
    dup2(write_pipefd[i][0], STDIN_FILENO); // asigno fd=0 a "read end"
    close(write_pipefd[i][0]); // cierro fd extra"

    close(read_pipefd[i][0]); // borro canal "read end"
    dup2(read_pipefd[i][1], STDOUT_FILENO); // asigno fd=1 a "write end"
    close(read_pipefd[i][1]); // cierro fd extra"
}