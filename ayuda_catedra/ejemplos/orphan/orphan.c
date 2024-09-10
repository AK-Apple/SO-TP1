// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
Ilustrar la creación de un proceso huérfano

Instrucciones:
make
./a.out &
ps j
<esperar primer sleep>
ps j
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  pid_t child_pid = fork();

  if (child_pid == -1) {
    perror("fork");
    return 1;
  }

  sleep(10);
  printf("First sleep done\n");

  if (child_pid == 0)
    sleep(10);
  else
    return 0;

  printf("First sleep done\n");

  return 0;
}
