#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char buffer[1024];

    // Set stdout to be line-buffered
    setvbuf(stdout, NULL, _IOLBF, 0);

    // Read lines until EOF is encountered
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        // Print a message to the master
        printf("%s random_md5 %d\n", buffer, getpid());
        fflush(stdout);
        if(strcmp(buffer, "EOF") == 0){
            break;
        }
    }

    // This message will be printed when EOF is received
    printf("Junior slave received EOF, exiting\n");
    fflush(stdout);

    exit(0);
}