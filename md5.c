/* Programa: md5.c aplication */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>


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


    // 1. Creo arrays de Pipes

    int write_pipefd[SLAVE_COUNT][2];
    int read_pipefd[SLAVE_COUNT][2];

    for(int i=0; i<SLAVE_COUNT; i++){
        if (pipe(write_pipefd[i]) == -1 || pipe(read_pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }  
    }

    // 2. Hago forks

    pid_t children_pid[SLAVE_COUNT] = {0}; 
    for(int i = 0; i < SLAVE_COUNT; i++) { 
        const char *slave_name = "junior_slave";    //TODO: reemplazar por el slave posta
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

    // 4. Hago la distribución inicial, es decir itero los archivos, y a cada archivo le asigno un slave
    //    canal para enviar cosas al slave:    write_pipefd[i][1]

    char message[100];  //de última lo alargamos
    int written_files = 0;
    int slave_it = 0;
    int file_limit = ((argc-1)*INIT_DISTRIB)/100;
    int local_to_read[SLAVE_COUNT] = {0};


    for(written_files=0; written_files < file_limit; written_files++){
        
        sprintf(message, "%s\n", argv[written_files+1]);

        write(write_pipefd[slave_it][1], message, strlen(message));
        printf("Enviado %s al esclavo %d\n", message, slave_it);
        local_to_read[slave_it]++;
        slave_it++;
        if (slave_it>=SLAVE_COUNT) slave_it = 0;
    }
    
    // sleep(1);

 

    int read_files = 0;

// ----------------------------  Start Main Loop -------------------------------

    while(read_files < argc-1){
        printf("new cycle. Read files: %d. Written files: %d\n", read_files, written_files);

        

        // 1. Create set of read pipes
        fd_set read_set;
        FD_ZERO(&read_set);
        int max_fd = 0;
        for(int i=0; i<SLAVE_COUNT; i++){
            FD_SET(read_pipefd[i][0], &read_set);
            if (read_pipefd[i][0] > max_fd) {
                max_fd = read_pipefd[i][0];
            }
        }

        // 2. Wait and select readable pipes
        struct timeval read_timeout = {0, 100000}; // 0.1 second timeout
        int readable_pipes = select(max_fd + 1, &read_set, NULL, NULL, &read_timeout);

        // 3. Iterate through readable pipes
        char buffer[4096];
        for(int i=0; i < SLAVE_COUNT && readable_pipes > 0 && read_files < argc-1; i++){
            if (FD_ISSET(read_pipefd[i][0], &read_set)) {
                ssize_t bytes_read = read(read_pipefd[i][0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';

                    char *token = strtok(buffer, "\n");
                    do{
                        // 3.1 Reads from slave and processes info

                        // --------- ¡¡¡ACÁ VIENE LO DE JAVI!!! ---------

                        printf("From slave %d: %s\n", i, token);

                        // --------- ¡¡¡ACÁ TERMINA LO DE JAVI!!! ---------

                        read_files++;
                        local_to_read[i]--;
                    } 
                    while ((token = strtok(NULL, "\n")));
                    readable_pipes--;

                    // write ONLY if slave has finished processing
                    if (local_to_read[i] == 0 && written_files < argc-1){
                        sprintf(message, "%s\n", argv[written_files+1]);
                        write(write_pipefd[i][1], message, strlen(message));
                        printf("Sent %s to slave %d\n", message, i);
                        written_files++;
                        local_to_read[i]++;
                    }
                }
            }
        }
    }

// ----------------------------  End Main Loop -------------------------------

    // sleep(1);
    
    // Close all pipes after processing
    for(int i=0; i<SLAVE_COUNT; i++){
        write(write_pipefd[i][1], "EOF\n", 4);
        close(write_pipefd[i][1]);  // This sends EOF to the slave
        close(read_pipefd[i][0]);   // Close the read end (optional at this point)
    }

    printf("Files left to write = %d\n", argc-1-written_files);
    printf("Files left to read = %d\n", argc-1-read_files);

    // 6. termina todo

    for(int i=0; i<SLAVE_COUNT; i++){
        waitpid(children_pid[i], NULL, 0);
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