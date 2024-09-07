

#ifndef PIPES_GROUP_ADT
#define PIPES_GROUP_ADT

#include <stdlib.h>


typedef struct PipeGroupCDT* PipeGroupADT;

// creates "size" pipe pairs (for reading and writing) and returns a pointer to the group
PipeGroupADT new_pipe_group(int size);

// chooses the i-th pipe pair and manipulates it so the program can read from the "read" pipe and write to the "write" pipe
void choose_pipe_pair(PipeGroupADT group, int i);

ssize_t write_pipe_pair(PipeGroupADT group, int i, char* str);

ssize_t read_pipe_pair(PipeGroupADT group, int i, char* buffer);

// returns '-1'-terminated array with selected pipe indexes
int* select_readable();

// sends EOF to all the pipes and closes them
void close_pipes(PipeGroupADT group);

#endif
