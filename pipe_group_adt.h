

#ifndef PIPES_GROUP_ADT
#define PIPES_GROUP_ADT

#include <stdlib.h>


typedef struct PipeGroupCDT* PipeGroupADT;

// creates "size" pipe pairs (for reading and writing) and returns a pointer to the group
PipeGroupADT new_pipe_group(int size);

// manipulates pipes so the process can read from the i-th "read" pipe and write to the i-th "write" pipe
void choose_pipe_pair(PipeGroupADT group, int i);

// should work like a write(2)
ssize_t write_pipe_pair(PipeGroupADT group, int i, char* str);

// should work like a read(2)
ssize_t read_pipe_pair(PipeGroupADT group, int i, char* buffer);

// puts in "selected" a '-1'-terminated array with selected pipe indexes
void select_readable(PipeGroupADT group, int* selected);

// sends EOF to all the pipes and closes them
void close_pipes(PipeGroupADT group);

#endif
