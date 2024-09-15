// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "result_adt.h"
#include "error_handling.h"

#define RESULT_FILE_PATH "./md5_result.txt"
#define SHARED_MEMORY_PATH_FORMAT "/shm%d"
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
    char full_buffer_semaphore_path[RESULT_PATH_SIZE];
} ResultCDT;


ResultADT new_result_adt(pid_t pid, int file_count) {
    ResultADT result_adt = (ResultADT) malloc(sizeof(ResultCDT));
    CHECK_NULL(result_adt);
    result_adt->result_file_fd = open(RESULT_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU); 
    result_adt->buffer_size = file_count * RESULT_STRING_SIZE;
    snprintf(result_adt->shared_memory_path, RESULT_PATH_SIZE, SHARED_MEMORY_PATH_FORMAT, pid);
    snprintf(result_adt->full_buffer_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_BUFFER_PATH_FORMAT, pid);
    
    result_adt->full_buffer_semaphore = sem_open(result_adt->full_buffer_semaphore_path, O_CREAT | O_EXCL, RW_PERMISSION, 0);
    if(result_adt->full_buffer_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_master");

    result_adt->shared_memory_fd = shm_open(result_adt->shared_memory_path, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if(result_adt->shared_memory_fd == -1)
        ERROR_EXIT("shm_open_master");

    if(ftruncate(result_adt->shared_memory_fd, result_adt->buffer_size + sizeof(long)) < 0)
        ERROR_EXIT("ftruncate");

    result_adt->shared_memory_base_adress = mmap(NULL, result_adt->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_adt->shared_memory_fd, 0);
    if(result_adt->shared_memory_base_adress == MAP_FAILED)
        ERROR_EXIT("mmap");
    result_adt->shared_memory_current_adress = result_adt->shared_memory_base_adress;

    return result_adt;
}

ResultADT open_result_adt(int pid, int file_count) {
    ResultADT result_adt = malloc(sizeof(ResultCDT));
    if(result_adt == NULL)
        ERROR_EXIT("malloc");

    result_adt->buffer_size = file_count * RESULT_STRING_SIZE;
    snprintf(result_adt->shared_memory_path, RESULT_PATH_SIZE, SHARED_MEMORY_PATH_FORMAT, pid); 
    snprintf(result_adt->full_buffer_semaphore_path, RESULT_PATH_SIZE, SEMAPHORE_BUFFER_PATH_FORMAT, pid);

    result_adt->full_buffer_semaphore = sem_open(result_adt->full_buffer_semaphore_path, 0, RW_PERMISSION, 0);
    if(result_adt->full_buffer_semaphore == SEM_FAILED)
        ERROR_EXIT("sem_open_vista_buffer");
    
    result_adt->shared_memory_fd = shm_open(result_adt->shared_memory_path, O_RDWR, S_IWUSR | S_IRUSR);
    if(result_adt->shared_memory_fd < 0)
        ERROR_EXIT("shm_open_vista");
    
    result_adt->shared_memory_base_adress = mmap(NULL, result_adt->buffer_size + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, result_adt->shared_memory_fd, 0);
    if(result_adt->shared_memory_base_adress == MAP_FAILED)
        ERROR_EXIT("mmap");
    result_adt->shared_memory_current_adress = result_adt->shared_memory_base_adress;

    return result_adt;
}

void free_result_adt(ResultADT result_adt) {
    char end_of_file[2] = {EOF, 0};
    write_result_adt(result_adt, end_of_file);
    sem_close(result_adt->full_buffer_semaphore);
    sem_unlink(result_adt->full_buffer_semaphore_path);
    shm_unlink(result_adt->shared_memory_path);
    close(result_adt->shared_memory_fd);
    close(result_adt->result_file_fd);
    munmap(result_adt->shared_memory_base_adress, result_adt->buffer_size + sizeof(long));
    free(result_adt);
}

int print_result_adt(ResultADT result_adt) {
    sem_wait(result_adt->full_buffer_semaphore);

    if(result_adt->shared_memory_current_adress[0] != (char)EOF) {
        printf("%s", result_adt->shared_memory_current_adress);
        result_adt->shared_memory_current_adress += strlen(result_adt->shared_memory_current_adress);
    }

    return result_adt->shared_memory_current_adress[0];
}

void write_result_adt(ResultADT result_adt, char* result) {
    int result_len = strlen(result);
    if(result[0] != (char)EOF)
        write(result_adt->result_file_fd, result, result_len);
    memcpy(result_adt->shared_memory_current_adress, result, result_len);
    result_adt->shared_memory_current_adress += result_len;

    sem_post(result_adt->full_buffer_semaphore);
}

void close_result_adt(ResultADT result_adt) {
    free(result_adt);
}