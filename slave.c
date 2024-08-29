#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    printf("slave no:%d\n", getpid());

// 1. TODO: strtok y read para separar
// 2. abrir un archivo
// 3. leer el archivo
// 4. TODO: calcular el hash md5 del archivo (investigar popen(3) y md5sum(shell))
// 5. enviar el hash md5 al master, es decir, imprimirlo

    char c;
    char input[100];  // Assuming input will not exceed 100 characters
    int i = 0;
    // Los esclavos reciben info con getchar
    while ((c = getchar()) != '\n' && i < 99) {
        input[i] = c;
        i++;
    }
    input[i] = '\0';  // Null-terminate the string

    // Open the file
    FILE *file = fopen(input, "rb");
    if (file == NULL) {
        printf("Error: Unable to open file '%s'\n", input);
        exit(1);
    }

    // Read and print the content of the file
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // close the file
    fclose(file);

    // printf("I'm %d and my master entered: %s\n", getpid(), input);

    exit(0);
}