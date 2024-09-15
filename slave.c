// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define MD5_LENGTH 256
#define MAX_BUF_SIZE 1024
#define TRUE 1

static void get_md5(const char* file_name, char* md5_sum);

int main() {
    int pid = getpid();
    char md5[MD5_LENGTH] = {0};

    while (TRUE) {
        char input[MAX_BUF_SIZE] = {0};
        ssize_t count = 0;
        count = read(STDIN_FILENO, &input, MAX_BUF_SIZE - 1);

        if (count <= 0 || input[0] == (char)EOF) {
            exit(0);
        }

        input[count] = '\0';

        char* token = strtok(input, "\n");
        while (token != NULL) {
            get_md5(token, md5);

            char message_buffer[MAX_BUF_SIZE] = {0};
            count = snprintf(message_buffer, MAX_BUF_SIZE, "%s %s %d\n", token, md5, pid);
            write(STDOUT_FILENO, message_buffer, count);

            token = strtok(NULL, "\n");
        }
    }
}

static void get_md5(const char* file_name, char* md5_sum) {
    char cmd[MAX_BUF_SIZE] = {0};
    snprintf(cmd, MAX_BUF_SIZE, "md5sum %s 2>/dev/null", file_name);

    FILE* md5_output_pipe = popen(cmd, "r");
    if (md5_output_pipe == NULL) return;

    while (isxdigit(*md5_sum++ = fgetc(md5_output_pipe)));
    *md5_sum = '\0';

    pclose(md5_output_pipe);
}
