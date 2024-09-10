// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define MD5_LENGTH 256
#define CMD_LENGTH 32
#define MAX_BUF_SIZE 1024
#define TRUE 1

void getMD5(const char *file_name, char *md5_sum);

int main()
{
    int pid = getpid();
    printf("slave no: %d\n", pid);

    char md5[MD5_LENGTH];

    while (TRUE) {

        char input[MAX_BUF_SIZE];
        size_t count = 0;
        count = read(STDIN_FILENO, &input, MAX_BUF_SIZE);

        if (count == 0 || input[0] == EOF){
             exit(0);
        }
           
        input[count-1] = '\0';
        printf("input: %s\n", input);

        char* token = strtok(input, "\n");
        while (token != NULL) {
            printf("entered strtok\n");
            getMD5(token, md5);

            char buff[MAX_BUF_SIZE];
            count = snprintf(buff, MAX_BUF_SIZE, "%s %s %d\n", token, md5, pid);
            write(STDOUT_FILENO, buff, count);

            token = strtok(NULL, "\n");
        }

    }
}

void getMD5(const char *file_name, char *md5_sum)
{
    char cmd[MAX_BUF_SIZE + strlen(file_name)];
    int count = sprintf(cmd, "md5sum %s 2>/dev/null", file_name);

    FILE * p = popen(cmd, "r");

    if (p == NULL) return;

    char c;
    for (int i = 0; i < count && isxdigit(c = fgetc(p)); i++) {
        *md5_sum++ = c;
    }

    *md5_sum = '\0';
    pclose(p);
}