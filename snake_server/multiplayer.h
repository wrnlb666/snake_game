#ifndef __MULTIPLAYER_H__
#define __MULTIPLAYER_H__


#include "falloc.h"
#include "frame.h"
#include "snake.h"



#define host 0
#define guest 1
#define tie 2




int check_snake(struct frame frame, snake snake_host, snake snake_guest);
char* p_to_c(position apple);
position c_to_p(char* message);
char* s_to_c(snake snake);
void c_to_s(snake *snake, char* message);









#endif