// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ResultADT.h"
#include "error_handling.h"
#include <sys/types.h>
#include <unistd.h>

#define RESULT_FILE_PATH "./md5_result.txt"
#define SHARED_MEMORY_PATH_FORMAT "/shm%d"
#define SEMAPHORE_MUTEX_PATH_FORMAT "/sem_mutex%d"
#define SEMAPHORE_BUFFER_PATH_FORMAT "/sem_buffer%d"
#define RW_PERMISSION 0660
#define RESULT_PATH_SIZE 256
#define RESULT_STRING_SIZE 128

typedef struct ResultCDT {
    char* shared_memory_current_adress;
    char* shared_memory_base_adress;
    sem_t* mutex_semaphore;
    sem_t* full_buffer_semaphore;
    size_t buffer_size;
    int shared_memory_fd;
    int result_file_fd;
    char shared_memory_path[RESULT_PATH_SIZE];
    char mutex_semaphore_path[RESULT_PATH_SIZE];
    char full_buffer_semaphore_path[RESULT_PATH_SIZE];
} ResultCDT;


ResultADT new_result_ADT(pid_t pid, int file_count) {
    ResultADT result_ADT = (ResultADT) malloc(sizeof(ResultCDT));
    CHECK_NULL(result_ADT);
    result_ADT->result_file_fd = open(RESULT_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU); 
    result_ADT->buffer_size = file_count * RESULT_STRING_SIZE;
    snprintf(result_ADT->shared_memory_path, RESULT_PATH_SIZE, SHARED_MEMORY_PATH_FORMAT, pid);
    snprintf(result_ADT->mutex_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_MUTEX_PATH_FORMAT, pid);
    snprintf(result_ADT->full_buffer_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_BUFFER_PATH_FORMAT, pid);

    result_ADT->mutex_semaphore = sem_open(result_ADT->mutex_semaphore_path, O_CREAT | O_EXCL, RW_PERMISSION, 1);
    if(result_ADT->mutex_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_master");
    
    result_ADT->full_buffer_semaphore = sem_open(result_ADT->full_buffer_semaphore_path, O_CREAT | O_EXCL, RW_PERMISSION, 1);
    if(result_ADT->full_buffer_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_master");

    result_ADT->shared_memory_fd = shm_open(result_ADT->shared_memory_path, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if(result_ADT->shared_memory_fd == -1)
        ERROR_EXIT("shm_open_master");

    if(ftruncate(result_ADT->shared_memory_fd, result_ADT->buffer_size + sizeof(long)) < 0)
        ERROR_EXIT("ftruncate");

    result_ADT->shared_memory_base_adress = mmap(NULL, result_ADT->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_ADT->shared_memory_fd, 0);
    if(result_ADT->shared_memory_base_adress == MAP_FAILED)
        ERROR_EXIT("mmap");
    result_ADT->shared_memory_current_adress = result_ADT->shared_memory_base_adress;

    return result_ADT;
}

ResultADT open_result_ADT(int pid, int file_count) {
    ResultADT result_ADT = malloc(sizeof(ResultCDT));
    if(result_ADT == NULL)
        ERROR_EXIT("malloc");

    result_ADT->buffer_size = file_count * RESULT_STRING_SIZE;
    snprintf(result_ADT->shared_memory_path, RESULT_PATH_SIZE, SHARED_MEMORY_PATH_FORMAT, pid); 
    snprintf(result_ADT->mutex_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_MUTEX_PATH_FORMAT, pid);
    snprintf(result_ADT->full_buffer_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_BUFFER_PATH_FORMAT, pid);

    result_ADT->mutex_semaphore = sem_open(result_ADT->mutex_semaphore_path, 0, RW_PERMISSION, 0);
    if(result_ADT->mutex_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_vista_mutex");

    result_ADT->full_buffer_semaphore = sem_open(result_ADT->full_buffer_semaphore_path, 0, RW_PERMISSION, 0);
    if(result_ADT->full_buffer_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_vista_buffer");
    
    result_ADT->shared_memory_fd = shm_open(result_ADT->shared_memory_path, O_RDWR, S_IWUSR | S_IRUSR);
    if(result_ADT->shared_memory_fd < 0)
        ERROR_EXIT("shm_open_vista");
    
    result_ADT->shared_memory_base_adress = mmap(NULL, result_ADT->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_ADT->shared_memory_fd, 0);
    if(result_ADT->shared_memory_base_adress == MAP_FAILED)
        ERROR_EXIT("mmap");
    result_ADT->shared_memory_current_adress = result_ADT->shared_memory_base_adress;

    return result_ADT;
}

void free_result_ADT(ResultADT result_ADT) {
    char end_of_file[2] = {EOF, 0};
    write_result(result_ADT, end_of_file);
    sem_close(result_ADT->full_buffer_semaphore);
    sem_close(result_ADT->mutex_semaphore);
    sem_unlink(result_ADT->full_buffer_semaphore_path);
    sem_unlink(result_ADT->mutex_semaphore_path);
    shm_unlink(result_ADT->shared_memory_path);
    close(result_ADT->shared_memory_fd);
    close(result_ADT->result_file_fd);
    munmap(result_ADT->shared_memory_base_adress, result_ADT->buffer_size + sizeof(long));
    free(result_ADT);
}

int print_result(ResultADT result_ADT) {
    // sem_wait(result_ADT->full_buffer_semaphore);
    sem_wait(result_ADT->mutex_semaphore);
    if(result_ADT->shared_memory_current_adress[0] != (char)EOF) {
        printf("%s", result_ADT->shared_memory_current_adress);
        result_ADT->shared_memory_current_adress += strlen(result_ADT->shared_memory_current_adress);
    }
    // sem_post(result_ADT->full_buffer_semaphore);
    return result_ADT->shared_memory_current_adress[0];
}

void write_result(ResultADT result_ADT, char *result) {
    // sem_wait(result_ADT->mutex_semaphore);
    int result_len = strlen(result);
    if(result[0] != (char)EOF)
        write(result_ADT->result_file_fd, result, result_len);
    memcpy(result_ADT->shared_memory_current_adress, result, result_len);
    result_ADT->shared_memory_current_adress += result_len;

    sem_post(result_ADT->mutex_semaphore);
    // sem_post(result_ADT->full_buffer_semaphore);
}


void close_result_ADT(ResultADT result_ADT) {
    free(result_ADT);
}