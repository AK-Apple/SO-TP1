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
    char input[MAX_BUF_SIZE];
    size_t count = 0;

    while (TRUE) {
        memset(input, 0, MAX_BUF_SIZE);
        memset(md5, 0, MD5_LENGTH);

        count = read(STDIN_FILENO, &input, MAX_BUF_SIZE);

        if (count <= 1)
            exit(0);

        input[count-1] = '\0';

        getMD5(input, md5);

        char buff[MAX_BUF_SIZE];
        
        count = snprintf(buff, MAX_BUF_SIZE, "%s - %s - %d\n", input, md5, pid);

        write(STDOUT_FILENO, buff, count);
        fflush(stdout);
        fflush(stdin);
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