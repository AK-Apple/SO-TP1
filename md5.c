/* Programa: md5.c aplication */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SLAVE_COUNT 5


int main(int argc, char *argv[])
{
    printf("Esto es md5.c: argcount=%d\n", argc);

    // "Debe iniciar los procesos esclavos."
    pid_t children_pid[SLAVE_COUNT] = {0}; 
    for(int i = 0; i < SLAVE_COUNT; i++) { 
        const char *slave_name = "slave";
        char *const param_list[] = {slave_name, NULL};
        pid_t child_pid = fork();
        if(child_pid == -1) {
            perror("fork");
            return 1;
        }
        else if(child_pid == 0) {
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

    puts("md5 application terminated successfully");
    return 0;
}
