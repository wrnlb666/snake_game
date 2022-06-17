#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "falloc.h"

int mymalloc_counter = 0;
struct malloc_queue *queue;


void* falloc(size_t size)
{
    void *address = malloc(size);
    struct malloc_node *new_node;
    
    if(mymalloc_counter == 0)
    {
        queue = malloc(sizeof(struct malloc_queue));
        queue->front = queue->back = NULL;
        new_node = malloc(sizeof(struct malloc_node));
        new_node->data = (unsigned long)address;
        new_node->next = NULL;
        queue->front = queue->back = new_node;
        atexit(falloc_cleanup);
        mymalloc_counter++;
    }
    else
    {
        new_node = malloc(sizeof(struct malloc_node));
        queue->back->next = new_node;
        new_node->data = (unsigned long)address;
        new_node->next = NULL;
        queue->back = queue->back->next;
        mymalloc_counter++;
    }

    return address;
}

void falloc_cleanup()
{
    void* temp_address;
    struct malloc_node *temp_node;
    while(queue->front->next != NULL)
    {
        temp_address = (void*)(queue->front->data);
        free(temp_address);
        temp_node = queue->front;
        queue->front = queue->front->next;
        free(temp_node);
    }
    temp_address = (void*)(queue->back->data);
    free(temp_address);
    free(queue->back);
}
