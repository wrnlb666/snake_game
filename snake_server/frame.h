#ifndef __FRAME_H__
#define __FRAME_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "falloc.h"


struct frame
{
    int width, height;
    char **frame;
};


void clear_screen();
void hide_cursor();
void show_cursor();
char** frame_init(struct frame frame);
void set_blank_frame(struct frame frame);
void print_screen(struct frame frame);







#endif