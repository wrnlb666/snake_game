#include "snake.h"
#include "frame.h"
#include "multisnake.h"



void single_player()
{
    struct frame frame;
    frame.height = 16;
    frame.width = 32;

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
                    if(snake.direction != down && snake.direction != up)
                    {
                        snake.direction = up;
                    }
                    break;
                }
                case ('s'):
                {
                    if(snake.direction != up && snake.direction != down)
                    {
                        snake.direction = down;
                    }
                    break;
                }                
                case ('a'):
                {
                    if(snake.direction != right && snake.direction != left)
                    {
                        snake.direction = left;
                    }
                    break;
                }
                case ('d'):
                {
                    if(snake.direction != left && snake.direction != right)
                    {
                        snake.direction = right;
                    }
                    break;
                }
                default:
                    break;
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
        strcpy(frame.frame[frame.height /2 -1] + (frame.width /2) - 5, "score: ");
        for(int i = 0; score[i] != '\n'; i++)
        {
            frame.frame[frame.height /2 -1][i + (frame.width /2) + 2] = score[i];
        }
    }
    print_screen(frame);
    sleep(5);

}






int main(int argc, char **argv)
{
    hide_cursor();
    srand(time(NULL));
    struct frame start_menu;
    start_menu.height = 16;
    start_menu.width = 32;
    if(argc == 3)
    {
        start_menu.width = atoi(argv[1]);
        start_menu.height = atoi(argv[2]);
        assert(start_menu.width >=32 && start_menu.height >= 16);
    }
    else if(argc != 1 && argc != 3)
    {
        fprintf(stderr, "usage: ./snake [width] [height]\n");
        sleep(3);
        abort();
    }

    start_menu.frame = frame_init(start_menu);
    
    start_over:
    clear_screen();

    set_blank_frame(start_menu);

    strncpy(start_menu.frame[start_menu.height /4], "1). Single Player", 17);
    strncpy(start_menu.frame[start_menu.height /2], "2). Multiplayer Hosting", 23);
    strncpy(start_menu.frame[(start_menu.height /4) *3], "3). Multiplayer Join", 20);

    print_screen(start_menu);

    while(1)
    {
        char ch = getch();
        switch(ch)
        {
            case (27):
                goto exit;

            case '1':
            {
                single_player();

                goto start_over;
                break;
            }
            case '2':
            {
                multiplayer_hosting();

                goto start_over;
                break;
            }
            case '3':
            {
                multiplayer_join();

                goto start_over;
                break;
            }
            default:
            {
                continue;
            }
        }
    }




    exit:
    show_cursor();
    EXIT_SUCCESS;
}



