#include <conio.h>
#include <Windows.h>

#include "snake.h"



int main(int argc, char **argv)
{
    srand(time(NULL));
    hide_cursor();
    struct frame frame;
    frame.height = 16;
    frame.width = 32;
    if(argc == 3)
    {
        frame.width = atoi(argv[1]);
        frame.height = atoi(argv[2]);
    }
    else if(argc != 1 && argc != 3)
    {
        fprintf(stderr, "usage: ./snake [width] [height]\n");
        sleep(3);
        abort();
    }

    frame.frame = frame_init(frame);

    snake snake;
    init_snake(&snake, frame, frame.height /2, frame.width /2);
    
    position apple;
    apple.x = apple.y = 0;
    new_apple(&apple, frame);
    check_apple(&apple, snake, snake, frame);

    clear_screen();

    while(1)
    {
        set_blank_frame(frame);
        render_snake(snake, frame, '#');
        render_apple(apple, frame);
        print_screen(frame);
        Sleep(1000 / FPS);

        if(kbhit())
        {
            char ch = getch();
            switch(ch)
            {
                case (27):
                    goto gameover;
                    break;

                case ('w'):
                {
                    if(snake.direction != down)
                    {
                        snake.direction = up;
                    }
                    break;
                }
                case ('s'):
                {
                    if(snake.direction != up)
                    {
                        snake.direction = down;
                    }
                    break;
                }                
                case ('a'):
                {
                    if(snake.direction != right)
                    {
                        snake.direction = left;
                    }
                    break;
                }
                case ('d'):
                {
                    if(snake.direction != left)
                    {
                        snake.direction = right;
                    }
                    break;
                }
                default:
                    continue;
            }
        }
        if(move_snake(&snake, frame, &apple))
        {
            goto gameover;
        }
    }
    //so that the pressing escape can jump out of the outer loop
    gameover:
    clear_screen();
    set_blank_frame(frame);
    char score[10];
    sprintf(score, "%d\n", snake.length -2);
    if(frame.width > 16)
    {
        strcpy(frame.frame[frame.height /2 -1] + (frame.width /2) -10, "score: ");
        for(int i = 0; score[i] != '\n'; i++)
        {
            frame.frame[frame.height /2 -1][i + (frame.width /2) - 3] = score[i];
        }
    }
    print_screen(frame);
    sleep(5);
    show_cursor();

    EXIT_SUCCESS;
}
