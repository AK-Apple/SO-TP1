#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    printf("slave\n");

// ------------- Créditos: ChatGPT ------------------ //

    char c;
    char input[100];  // Assuming input will not exceed 100 characters
    int i = 0;

    // Los esclavos reciben info con getchar
    while ((c = getchar()) != '\n' && i < 99) {
        input[i] = c;
        i++;
    }
    input[i] = '\0';  // Null-terminate the string

    //Los esclavos imprimen al master con printf
    printf("I'm %d and my master entered: %s\n", getpid(), input);

// ------------- Créditos: ChatGPT ------------------ //

    exit(0);
}