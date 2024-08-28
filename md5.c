/* Programa: md5.c aplication */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define SLAVE_COUNT 5
#define INIT_DISTRIB 10  // representa un porcentaje (10%)


void manipulate_pipes(int write_pipefd[SLAVE_COUNT][2], int read_pipefd[SLAVE_COUNT][2], int i);

// Ejemplo:
// Tengo 100 esclavos
// Seteo INIT_DISTRIB = 10
// Seteo SLAVE_COUNT = 5
// Entonces se distribuyen 10 archivos en 5 esclavos

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

    // Hago la distribución inicial, es decir itero los archivos, y a cada archivo le asigno un slave
    //    canal para enviar cosas al slave:    write_pipefd[i][1]

    char message[100];  //de última lo alargamos
    int file_it = 0;
    int slave_it = 0;
    int file_limit = (argc-1)*INIT_DISTRIB/100;

    // TODO: Ver si hay alguna mejor forma de iterar
    for(file_it=0; file_it < file_limit; file_it++){
        
        // sprintf(message, "Mensaje numero %d\n", i);
        // sprintf(message, "vista.c\n");

        write(write_pipefd[slave_it][1], argv[file_it+1], strlen(argv[file_it+1]));
        slave_it++;
        if (slave_it==SLAVE_COUNT) slave_it = 0;
    }

    // Acá iría el slave
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