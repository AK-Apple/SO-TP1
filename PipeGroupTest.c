#include "shared_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#include "PipeGroupADT.c"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


#define INIT_DISTRIB 10  // representa un porcentaje (10%)

// NUEVOS:
#define SLAVES_PER_100_FILES 5
#define MAX_FILES_PER_SLAVE 2
#define MAX_SLAVES_COUNT 20

#define VIEW_SLEEP 2

int instances_of_char(char* str, char c);

void manipulate_pipes(int write_pipefd[][2], int read_pipefd[][2], int i);

int ceil_of_fraction(int x, int y);

// Ejemplo:
// Tengo 100 esclavos
// Seteo SLAVES_PER_100_FILES = 10
// Seteo POSSIBLE_FILES_PER_SLAVE = 5

// Entonces se distribuyen 10 archivos en 5 esclavos

int main(int argc, char *argv[]) {
    printf("Esto es md5.c: argcount=%d\n", argc);

    int files_amount = argc - 1;
    int slaves_count = MIN(ceil_of_fraction(files_amount * SLAVES_PER_100_FILES, 100), MAX_SLAVES_COUNT);
    int files_per_slave = MIN(files_amount/slaves_count, MAX_FILES_PER_SLAVE);

    printf("slaves_count = %d\nfiles_per_slave = %d\n", slaves_count, files_per_slave);
    

    

    const char *result_file_name = "result.txt";
    int result_fd = open(result_file_name, (O_RDWR | O_CREAT | O_TRUNC), S_IRWXU);


    PipeGroupADT pipe_group = new_pipe_group(slaves_count);

    // 2. Forks
    pid_t children_pid[MAX_SLAVES_COUNT] = {0}; 
    for(int i = 0; i < slaves_count; i++) { 
        const char *slave_name = "slave";    //TODO: reemplazar por el slave posta
        char * const param_list[2] = {slave_name, NULL};
        pid_t child_pid = fork();
        if(child_pid == -1) {
            perror("fork");
            return 1;
        }
        else if(child_pid == 0) {

            // 3. Manipulo pipes
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

    // shared memory proceso vista.c
    char *shared_memory_path = "/shared_mem94";

    // --------- Comado  importate ----------
    shm_unlink(shared_memory_path);
    // --------- Comado  importate ----------

    /* Create shared memory object and set its size to the size
        of our structure */
    int shared_memory_fd = shm_open(shared_memory_path, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (shared_memory_fd == -1)
        errExit("shm_open");
    if (ftruncate(shared_memory_fd, sizeof(SharedInfo)) == -1)
        errExit("ftruncate");
    /* Map the object into the caller's address space */
    SharedInfo *shared_memory_pointer = mmap(NULL, sizeof(*shared_memory_pointer), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_pointer == MAP_FAILED)
        errExit("mmap");
    if (sem_init(&shared_memory_pointer->semaphore, 1, 0) == -1)
        errExit("sem_init-sem");
    write(STDOUT_FILENO, shared_memory_path, strlen(shared_memory_path)); // print shared_memory_path for | vista.c


    // 4. Hago la distribución inicial, es decir itero los archivos, y a cada archivo le asigno un slave
    //    canal para enviar cosas al slave:    write_pipefd[i][1]

    char message[100];  //de última lo alargamos
    int slave_it = 0;
    int file_limit = ((argc-1)*INIT_DISTRIB)/100;
    int local_to_read[MAX_SLAVES_COUNT] = {0};



    for(int i=0; i<slaves_count; i++){
        for(int j=0; j<files_per_slave; j++){
            int written_files = i*files_per_slave + j;
            sprintf(message, "%s\n", argv[written_files+1]);
            write_pipe_pair(pipe_group, i, message);
            printf("Enviado %s al esclavo %d\n", message, i);
        }
        local_to_read[i] = files_per_slave;
    }


    sleep(VIEW_SLEEP); // consigna: Cuando inicia, debe esperar 2 segundos a que aparezca un proceso vista, si lo hace le comparte el buffer de llegada


    int read_files = 0;
    int written_files = slaves_count*files_per_slave;

// ----------------------------  Start Main Loop -------------------------------

    while(read_files < argc-1){
        printf("new cycle. Read files: %d. Written files: %d\n", read_files, written_files);

        // 1. Create set of read pipes
        // 2. Select readable pipes
        int* selected = select_readable(pipe_group);

        // 3. Iterate through readable pipes
        char buffer[4096];

        for(int j=0; selected[j]!=-1 && read_files < argc-1; j++){
            int i = selected[j];
            
            ssize_t bytes_read = read_pipe_pair(pipe_group, i, buffer);
            buffer[bytes_read] = '\0';

            // Start Javi //

            sem_post(&shared_memory_pointer->semaphore);

            // End Javi //

            write(result_fd, buffer, strlen(buffer));

            int files_this_iteration = instances_of_char(buffer, '\n');
            read_files+= files_this_iteration;
            local_to_read[i]-= files_this_iteration;


            // write ONLY if slave has finished processing
            if (local_to_read[i] == 0 && written_files < argc-1){
                sprintf(message, "%s\n", argv[written_files+1]);
                write_pipe_pair(pipe_group, i, message);
                // printf("Sent %s to slave %d\n", message, i);
                written_files++;
                local_to_read[i]++;
            }


        }
    }

// ----------------------------  End Main Loop -------------------------------

    printf("end main loop\n");
    
   close_pipes(pipe_group);

    // printf("Files left to write = %d\n", argc-1-written_files);
    // printf("Files left to read = %d\n", argc-1-read_files);

    // 6. termina todo

    for(int i=0; i<slaves_count; i++){
        waitpid(children_pid[i], NULL, 0);
    }


    shared_memory_pointer->buf[0].md5[0] = ';';
    sem_post(&shared_memory_pointer->semaphore);
    sem_destroy(&shared_memory_pointer->semaphore);
    shm_unlink(shared_memory_path);
    close(result_fd);

    // puts("md5 application terminated successfully");
    // printf("random child_pid to avoid warning: %d\n", children_pid[0]);

    return 0;
}


int instances_of_char(char* str, char c){
    int i, count;
    for (i=0, count=0; str[i]; i++)
        count += (str[i] == c);
    return(count);
}

int ceil_of_fraction(int x, int y){
    return (x + y - 1) / y;
}
