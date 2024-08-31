/* Programa: Hola mundo */
#include "shared_info.h"

int
main(int argc, char *argv[])
{
    char shared_memory_path[256];
    if (argc == 1) {
        read(STDIN_FILENO, shared_memory_path, 256);
    }
    else {
        strcpy(shared_memory_path, argv[1]); // copia quiza innecesaria
    }
    // printf("shgPATH [%s]\n", shared_memory_path);

    /* Open the existing shared memory object and map it
        into the caller's address space */

    int shared_memory_fd = shm_open(shared_memory_path, O_RDWR, 0);
    if (shared_memory_fd == -1)
        errExit("shm_open");

    SharedInfo *shared_memory_pointer = mmap(NULL, sizeof(*shared_memory_pointer), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_pointer == MAP_FAILED)
        errExit("mmap");

    sem_post(&shared_memory_pointer->sem1);

    if (sem_wait(&shared_memory_pointer->sem2) == -1)
        errExit("sem_wait");
    /* Write modified data in shared memory to standard output */
    while(shared_memory_pointer->buf[0].md5[0] != ';') {
        if (sem_wait(&shared_memory_pointer->sem2) == -1)
            errExit("sem_wait");
        printf("vista [md5:%s] [id:%d] [name:%s]\n", shared_memory_pointer->buf[0].md5, shared_memory_pointer->buf[0].id, shared_memory_pointer->buf[0].name);
    }


    exit(EXIT_SUCCESS);
}