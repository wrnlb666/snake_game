#include "multiplayer.h"








int check_snake(struct frame frame, snake snake_host, snake snake_guest)
{
    //check host death
    for(int i = 0; i < snake_guest.length -1; i++)
    {
        if(snake_host.head.x == snake_guest.body[i].x && snake_host.head.y == snake_guest.body[i].y)
        {
            return host;
        }
    }
    if(snake_host.head.x < 0 || snake_host.head.x == frame.width || snake_host.head.y < 0 || snake_host.head.y == frame.height)
    {
        return host;
    }


    //check guest death
    for(int i = 0; i < snake_host.length -1; i++)
    {
        if(snake_guest.head.x == snake_host.body[i].x && snake_guest.head.y == snake_host.body[i].y)
        {
            return guest;
        }
    }
    if(snake_guest.head.x < 0 || snake_guest.head.x == frame.width || snake_guest.head.y < 0 || snake_guest.head.y == frame.height)
    {
        return guest;
    }


    //if heads corrupted
    if(snake_host.head.x == snake_guest.head.x && snake_host.head.y == snake_guest.head.y)
    {
        if(snake_host.length > snake_guest.length)
        {
            return guest;
        }
        else if(snake_host.length < snake_guest.length)
        {
            return host;
        }
        else
        {
            //same length. tie
            return 2;
        }
    }
}




//convert apple's position to message friendly format
char* p_to_c(position apple)
{
    assert(apple.x < 256 && apple.y < 256);
    
    char *message = malloc(sizeof(char) *2);
    message[0] = apple.x;
    message[1] = apple.y;

    return message;
}

//convert message back to apple's position
position c_to_p(char* message)
{
    position apple;
    apple.x = message[0];
    apple.y = message[1];

    return apple;
}




//convert snake's status to message friendly format
char* s_to_c(snake snake)
{
    char *message = malloc(sizeof(char) * (5 + (64 * 32 * 2)));

    message[0] = snake.direction;
    message[1] = snake.head.x;
    message[2] = snake.head.y;
    //since the maxium number of snake.length can be 2048, make it 2 chars
    int length1, length2;
    length1 = snake.length /256;
    length2 = snake.length %256;
    message[3] = length1;
    message[4] = length2;

    for(int i = 5, j = 0; j < snake.length -1; i += 2, j++)
    {
        message[i] = snake.body[j].x;
        message[i+1] = snake.body[j].y;
    }

    return message;
}




//convert message back to snake's status
void c_to_s(snake *snake, char* message)
{
    snake->direction = message[0];
    snake->head.x = message[1];
    snake->head.y = message[2];
    //merge 2 chars back to snake.length
    snake->length = ((message[3] * 256) + message[4]);

    for(int i = 5, j = 0; j < snake->length -1; i += 2, j++)
    {
        snake->body[j].x = message[i];
        snake->body[j].y = message[i+1];
    }
}
