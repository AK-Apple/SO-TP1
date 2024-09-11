#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#define ERROR_EXIT(msg)     do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define CHECK_NULL(x)       if((x) == NULL) ERROR_EXIT("malloc")

#endif