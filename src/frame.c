#include "frame.h"


void clear_screen()
{
    system("cls");
}


void hide_cursor()
{
    printf("\e[?25l");
}


void show_cursor()
{
    printf("\e[?25h");
}



char** frame_init(struct frame frame)
{
    assert(frame.height >= 10 && frame.width >= 10);

    //allocate memory for struct frame
    frame.frame = falloc(sizeof(char*) * frame.height);
    for(int i = 0; i < frame.height; i++)
    {
        frame.frame[i] = falloc(sizeof(char) * frame.width);
    }

    return frame.frame;
}



//setting everything to a space
void set_blank_frame(struct frame frame)
{
    for(int i = 0; i < frame.height; i++)
    {
        for(int j = 0; j < frame.width; j++)
        {
            frame.frame[i][j] = ' ';
        }
    }
}


int clear_screen_counter = 0;
void print_screen(struct frame frame)
{
    //call clean when exits
    if(clear_screen_counter == 0)
    {
        atexit(clear_screen);
        clear_screen_counter++;
    }

    //print screen
    for(int i = 0; i < frame.width +2; i++)
    {
        printf("* ");
    }
    putchar('\n');
    
    for(int i = 0; i < frame.height; i++)
    {
        printf("* ");
        for(int j = 0; j < frame.width; j++)
        {
            printf("%c ", frame.frame[i][j]);
            fflush(stdout);
        }
        printf("*\n");
    }

    for(int i = 0; i < frame.width +2; i++)
    {
        printf("* ");
    }

    //changing cursor position(next frame)
    printf("\x1b[%dD", (frame.width +2) *2);
    printf("\x1b[%dA", frame.height + 1);
}


