#include "resultADT.h"

typedef struct ResultCDT {
    int    result_file_fd;
    char *shared_memory_base_adress;
    sem_t *mutex_semaphore;
    sem_t *full_buffer_semaphore;
    size_t buffer_size;
    int shared_memory_fd;
    char shared_memory_path[SHM_PATH_MAX];
    char mutex_semaphore_path[SHM_PATH_MAX];
    char full_buffer_semaphore_path[SHM_PATH_MAX];
} ResultCDT;


ResultADT new_result_ADT(int pid) {
    ResultADT result_ADT = (ResultADT) malloc(sizeof(ResultCDT));
    result_ADT->result_file_fd = open("/result/resultOF.txt", (O_RDWR | O_CREAT | O_TRUNC), S_IRWXU); // TODO
    if(result_ADT == NULL)
        errExit("malloc");
    result_ADT->buffer_size = BUFFER_SIZE;
    sprintf(result_ADT->shared_memory_path, "/shm%d", pid);
    sprintf(result_ADT->mutex_semaphore_path, "/sem_mutex%d", pid);
    sprintf(result_ADT->full_buffer_semaphore_path, "/sem_buffer%d", pid);

    // unlink previous opcinoal

    result_ADT->mutex_semaphore = sem_open(result_ADT->mutex_semaphore_path, O_CREAT | O_EXCL, 0660, 1);
    if(result_ADT->mutex_semaphore == SEM_FAILED)
        errExit("sem_open_master");
    
    result_ADT->full_buffer_semaphore = sem_open(result_ADT->full_buffer_semaphore_path, O_CREAT | O_EXCL, 0660, 1);
    if(result_ADT->full_buffer_semaphore == SEM_FAILED)
        errExit("sem_open_master");

    result_ADT->shared_memory_fd = shm_open(result_ADT->shared_memory_path, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if(result_ADT->shared_memory_fd == -1)
        errExit("shm_open_master");

    if(ftruncate(result_ADT->shared_memory_fd, result_ADT->buffer_size + sizeof(long)) < 0)
        errExit("ftruncate");

    result_ADT->shared_memory_base_adress = mmap(NULL, result_ADT->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_ADT->shared_memory_fd, 0);
    if(result_ADT->shared_memory_base_adress == MAP_FAILED)
        errExit("mmap");
    sleep(2);
    return result_ADT;
}

ResultADT open_result_ADT(int pid) {
    ResultADT result_ADT = malloc(sizeof(ResultCDT));

    result_ADT->buffer_size = BUFFER_SIZE;
    sprintf(result_ADT->shared_memory_path, "/shm%d", pid); // TODO codigo repetido
    sprintf(result_ADT->mutex_semaphore_path, "/sem_mutex%d", pid);
    sprintf(result_ADT->full_buffer_semaphore_path, "/sem_buffer%d", pid);

    result_ADT->mutex_semaphore = sem_open(result_ADT->mutex_semaphore_path, 0, 0660, 0);
    if(result_ADT->mutex_semaphore == SEM_FAILED)
        errExit("sem_open_vista_mutex");

    result_ADT->full_buffer_semaphore = sem_open(result_ADT->full_buffer_semaphore_path, 0, 0660, 0);
    if(result_ADT->full_buffer_semaphore == SEM_FAILED)
        errExit("sem_open_vista_buffer");
    
    result_ADT->shared_memory_fd = shm_open(result_ADT->shared_memory_path, O_RDWR, S_IWUSR | S_IRUSR);
    if(result_ADT->shared_memory_fd < 0)
        errExit("shm_open_vista");
    
    result_ADT->shared_memory_base_adress = mmap(NULL, result_ADT->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_ADT->shared_memory_fd, 0);
    if(result_ADT->shared_memory_base_adress == MAP_FAILED)
        errExit("mmap");
    
    return result_ADT;
}

void free_result_ADT(ResultADT result_ADT) {
    write_result(result_ADT, ";");
    sem_destroy(result_ADT->full_buffer_semaphore);
    sem_destroy(result_ADT->mutex_semaphore);
    shm_unlink(result_ADT->shared_memory_path);
    close(result_ADT->shared_memory_fd);
    munmap(result_ADT, sizeof(ResultCDT) + sizeof(long));
    // TODO free close, etc
}

int print_result(ResultADT result_ADT) {

    // sem_wait(result_ADT->full_buffer_semaphore);
    sem_wait(result_ADT->mutex_semaphore);

    puts(result_ADT->shared_memory_base_adress);

    // sem_post(result_ADT->full_buffer_semaphore);
    return result_ADT->shared_memory_base_adress[0];
}

void write_result(ResultADT result_ADT, char *result) {

    // sem_wait(result_ADT->mutex_semaphore);
    int len = strlen(result);

    // puts(result);
    write(result_ADT->result_file_fd, result, len);
    memcpy(result_ADT->shared_memory_base_adress, result, len);

    sem_post(result_ADT->mutex_semaphore);
    // sem_post(result_ADT->full_buffer_semaphore);
}


void close_result_ADT(ResultADT Result_ADT) {
    // TODO free
}