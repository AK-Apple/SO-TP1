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


int main() {
    char buffer[1024];

    // Set stdout to be line-buffered
    setvbuf(stdout, NULL, _IOLBF, 0);

    // Read lines until EOF is encountered
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if(strcmp(buffer, "EOF") == 0){
            break;
        }
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        // Print a message to the master

        char md5[MD5_LENGTH];
        getMD5(buffer, md5);
        printf("%s %s %d\n", buffer, md5, getpid());
        fflush(stdout);
    }

    // This message will be printed when EOF is received
    printf("Junior slave received EOF, exiting\n");
    fflush(stdout);

    exit(0);
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