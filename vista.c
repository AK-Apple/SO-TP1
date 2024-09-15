// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "result_adt.h"
#include "error_handling.h"

int main(int argc, char* argv[]) {
    int pid = 0;
    int file_count = 1;
    if (argc == 1) {
        int result = scanf("%d %d", &pid, &file_count);
        if (result != 2) {
            ERROR_EXIT("invalid input, expected <master_pid> <file_count> from stdin");
        }
    }
    else if (argc == 3) {
        pid = atoi(argv[1]);
        file_count = atoi(argv[2]); 
    }
    else {
        ERROR_EXIT("Expected 3 or 1 arguments but got \nusage: ./vista <master_pid> <file_count>");
    }

    ResultADT result_adt = open_result_adt(pid, file_count);
    int res = 0;
    for (int i = 0; res != EOF && i < file_count; i++) {
        res = print_result_adt(result_adt);
    }

    close_result_adt(result_adt);
    exit(EXIT_SUCCESS);
}
