// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// c.c
// Prints the stdin on stdout while counting the lines

#include <stdio.h>

int main() {
  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  int linecount = 0;

  while ((linelen = getline(&line, &linecap, stdin)) > 0) {
    linecount++;
    printf("Line %d: ", linecount);
    fwrite(line, linelen, 1, stdout);
  }

  return 0;
}
