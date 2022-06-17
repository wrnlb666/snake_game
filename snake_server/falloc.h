#include <stdio.h>
#include <stdlib.h>

#ifndef __FALLOC_H__
#define __FALLOC_H__

struct malloc_node
{
    unsigned long data;
    struct malloc_node *next;
};

struct malloc_queue
{
    struct malloc_node *front, *back;
};


void* falloc(size_t size);
void falloc_cleanup();

#endif