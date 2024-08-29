#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define MD5_LENGTH 256
#define CMD_LENGTH 32

void getMD5(const char *file_name, char *md5_sum);

int main()
{
    printf("slave no:%d\n", getpid());

    // 1. TODO: strtok y read para separar
    // 2. abrir un archivo
    // 3. leer el archivo
    // 4. TODO: calcular el hash md5 del archivo (investigar popen(3) y md5sum(shell))
    // 5. enviar el hash md5 al master, es decir, imprimirlo


    char input[100];  // Assuming input will not exceed 100 characters
    int i = 0;
    char c;
    // Los esclavos reciben info con getchar
    while ((c = getchar()) != '\n' && i < 99) {
        input[i] = c;
        i++;
    }
    input[i] = '\0';  // Null-terminate the string

    char md5[MD5_LENGTH + 1];

    while (1) {
        getMD5(input, md5);
        printf("%s\n", md5);
    }

    exit(0);
}

void getMD5(const char *file_name, char *md5_sum)
{
    char cmd[CMD_LENGTH + sizeof(*file_name)];
    sprintf(cmd, "md5sum %s 2>/dev/null", file_name);

    FILE *p = popen(cmd, "r");

    if (p == NULL) return;

    int i;
    char c;
    for (i = 0; i < MD5_LENGTH && isxdigit(c = fgetc(p)); i++) {
        *md5_sum++ = c;
    }

    *md5_sum = '\0';
    pclose(p);
}