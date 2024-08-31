#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define MD5_LENGTH 256
#define CMD_LENGTH 32

void getMD5(const char *file_name, char *md5_sum);

int main()
{
    int pid = getpid();
    printf("slave no: %d\n", pid);

    char md5[MD5_LENGTH + 1] = {0};
    char input[1024] = {0};
    size_t count = 0;

    while (1) {
        fflush(STDIN_FILENO);
        count = read(STDIN_FILENO, &input, 1024);

        if (*input == EOF) exit(0);

        input[count-1] = '\0';

        getMD5(input, md5);
        printf("%s - %s - %d\n", input, md5, pid);
    }
}

void getMD5(const char *file_name, char *md5_sum)
{
    char cmd[CMD_LENGTH + sizeof(*file_name)];
    sprintf(cmd, "md5sum %s 2>/dev/null", file_name);

    FILE * p = popen(cmd, "r");

    if (p == NULL) return;

    char c;
    for (int i = 0; i < MD5_LENGTH && isxdigit(c = fgetc(p)); i++) {
        *md5_sum++ = c;
    }

    *md5_sum = '\0';
    pclose(p);
}