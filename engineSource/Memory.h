#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

struct mem_Component
{
    struct list *memList;
    long int currentBytesAllocated;
    long int maxBytesAllocated;
    char memoryLeaked;

    FILE *log;
};

struct mem_Info
{
    const char *file;
    unsigned int line;
    unsigned int bytes;
};

void mem_Reset(void);
int mem_Init(void);

void *mem_MallocD(unsigned int bytes,const int line);
void *mem_FreeD(void *memory,unsigned int bytes,const int line);

#define mem_Alloc(bytes) mem_Malloc(bytes, __LINE__, __FILE__)
void *mem_Malloc(unsigned int bytes, const int line, const char *file);

void *mem_Free(void *memory);

void mem_Clear(void);

int mem_Report(void);

int mem_Quit(void);

#endif
