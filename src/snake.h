#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "frame.h"
#include "falloc.h"

#define FPS 6

typedef struct snake_position
{
    int y;
    int x;
}position;


typedef enum direction
{
    up, down, left, right
}direction;


typedef struct snake
{
    direction direction;
    position head;
    int length;
    position *body;
}snake;


void init_snake(snake *snake, struct frame frame, int y, int x);
void render_snake(snake snake, struct frame frame, char symbol);
void new_apple(position *apple, struct frame frame);
void check_apple(position *apple, snake snake1, snake snake2, struct frame frame);
int move_snake(snake *snake, struct frame frame, position *apple);
void render_apple(position apple, struct frame frame);





#endif