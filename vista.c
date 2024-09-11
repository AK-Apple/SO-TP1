// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ResultADT.h"

int main(int argc, char* argv[]) {
    int pid = 0;
    int file_count = 1;
    if (argc == 1) {
        int result = scanf("%d %d", &pid, &file_count);
        if(result != 2) {
            fprintf(stderr, "invalid input, expected <master_pid> <file_count>\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(argc == 3) {
        pid = atoi(argv[1]);
        file_count = atoi(argv[2]); 
    }
    else {
        fprintf(stderr, "Expected 3 or 1 arguments but got %d\n%s <master_pid> <file_count>\n", argc, argv[0]);
        exit(EXIT_FAILURE);
    }

    ResultADT result_ADT = open_result_ADT(pid, file_count);
    int res = 0;
    while(res != EOF) {
        res = print_result(result_ADT);
    }

    close_result_ADT(result_ADT);
    exit(EXIT_SUCCESS);
}