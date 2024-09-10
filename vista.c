#include "resultADT.h"

int
main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); 
    char shared_memory_path[256];
    if (argc == 1) {
        scanf("%s", shared_memory_path);
        // read(STDIN_FILENO, shared_memory_path, 256);
    }
    else {
        strcpy(shared_memory_path, argv[1]); // copia quiza innecesaria
    }
    int pid = atoi(shared_memory_path);

    ResultADT result_ADT = open_result_ADT(pid);
    int res = 0;
    while(res != ';') {
        res = print_result(result_ADT);
    }

    close_result_ADT(result_ADT);
    exit(EXIT_SUCCESS);
}