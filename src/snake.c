#include "snake.h"


void init_snake(snake *snake, struct frame frame, int y, int x)
{
    snake->body = falloc(sizeof(position) * frame.height * frame.width);
    snake->length = 2;
    snake->direction = rand()%4;
    snake->head.y = y;
    snake->head.x = x;
    switch(snake->direction)
    {
        case up:
        {
            snake->body[0].y = snake->head.y +1;
            snake->body[0].x = snake->head.x;
            break;
        }
        case down:
        {
            snake->body[0].y = snake->head.y -1;
            snake->body[0].x = snake->head.x;
            break;
        }
        case left:
        {
            snake->body[0].y = snake->head.y;
            snake->body[0].x = snake->head.x -1;
            break;
        }
        case right:
        {
            snake->body[0].y = snake->head.y;
            snake->body[0].x = snake->head.x +1;
            break;
        }
    }
}




void render_snake(snake snake, struct frame frame, char symbol)
{
    frame.frame[snake.head.y][snake.head.x] = symbol;
    for(int i = 0; i < snake.length -1; i++)
    {
        frame.frame[snake.body[i].y][snake.body[i].x] = '+';
    }
}




void new_apple(position *apple, struct frame frame)
{
    apple->x = 0;
    apple->y = 0;
    apple->x = 1+ rand()%(frame.width -1);
    apple->y = 1+ rand()%(frame.height -1);
}


void check_apple(position *apple, snake snake1, snake snake2, struct frame frame)
{
    //head check
    if((apple->x == snake1.head.x && apple->y == snake1.head.y) || (apple->x == snake2.head.x && apple->y == snake2.head.y))
    {
        new_apple(apple, frame);
        check_apple(apple, snake1, snake2, frame);
    }
    //body check
    for(int i = 0; i < snake1.length -1; i++)
    {
        if(apple->x == snake1.body[i].x && apple->y == snake1.body[i].y)
        {
            new_apple(apple, frame);
            check_apple(apple, snake1, snake2, frame);
        }
    }
    for(int i = 0; i < snake2.length -1; i++)
    {
        if(apple->x == snake2.body[i].x && apple->y == snake2.body[i].y)
        {
            new_apple(apple, frame);
            check_apple(apple, snake1, snake2, frame);
        }
    }
}




int move_snake(snake *snake, struct frame frame, position *apple)
{
    position tail = snake->body[snake->length -2];
    
    //move body
    for(int i = snake->length -2; i >= 0; i--)
    {
        if(i == 0)
        {
            snake->body[i].x = snake->head.x;
            snake->body[i].y = snake->head.y;
        }
        else
        {
            snake->body[i].x = snake->body[i-1].x;
            snake->body[i].y = snake->body[i-1].y;
        }
    }

    //move head
    switch(snake->direction)
    {
        case up:
        {
            snake->head.y -= 1;
            break;
        }
        case down:
        {
            snake->head.y += 1;
            break;
        }
        case left:
        {
            snake->head.x -= 1;
            break;
        }
        case right:
        {
            snake->head.x += 1;
            break;
        }
    }

    //apple detection
    if(snake->head.x == apple->x && snake->head.y == apple->y)
    {
        snake->body[snake->length -1] = tail;
        snake->length++;
        
        
        new_apple(apple, frame);
        check_apple(apple, *snake, *snake, frame);
    }

    //game over detection
    if(snake->head.x < 0 || snake->head.x == frame.width || snake->head.y < 0 || snake->head.y == frame.height)
    {
        return 1;
    }
    for(int i = 0; i < snake->length -1; i++)
    {
        if(snake->head.x == snake->body[i].x && snake->head.y == snake->body[i].y)
        {
            return 1;
        }
    }
    return 0;
}



void render_apple(position apple, struct frame frame)
{
    frame.frame[apple.y][apple.x] = '@';
}



