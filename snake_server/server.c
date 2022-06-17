#include "falloc.h"
#include "frame.h"
#include "snake.h"
#include "multiplayer.h"

#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define DEFAULT_PORT 50001

#define player1 1
#define player2 2


//debug
#define debug printf("%d\n\n", __LINE__);fflush(stdout)




//Global variables for receiving and sending in threads
//allocate frame
struct frame frame;

//allocate memory for snakes
void *message_1, *message_2;
char *snake_message_1, *snake_message_2;

//allocate memory for apples
char *apple_c;
position apple;


//socket
int psock1, psock2;







void* recv_from_client(void *void_sock_fd)
{
    int sockfd = *(int*)void_sock_fd;
    int recv_len = 0;
    char *snake_message;
    snake_message = malloc(sizeof(char) * (5 + (64 * 32 * 2)));
    recv_len = recv(sockfd, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
    while(recv_len < sizeof(char) * (5 + (64 * 32 * 2)))
    {
        recv_len += recv(sockfd, snake_message + recv_len, sizeof(char) * (5 + (64 * 32 * 2)) - recv_len, 0);
    }
    //printf("[S]: %d bytes received\n", recv_len);
    //fflush(stdout);
    if(sockfd == psock1)
    {
        snake_message_1 = snake_message;
    }
    else if(sockfd == psock2)
    {
        snake_message_2 = snake_message;
    }
    else
    {
        fprintf(stderr, "[S]: Thread socket error, aborted\n");
        abort();
    }
}




void* send_apple(void *void_sock_fd)
{
    int sockfd = *(int*)void_sock_fd;
    apple_c = p_to_c(apple);
    send(sockfd, apple_c, 2, 0);
    free(apple_c);
}



void* send_to_client(void *void_sock_fd)
{
    int sockfd = *(int*)void_sock_fd;
    int send_len = 0;
    char *snake_message;

    //send snake message
    if(sockfd == psock1)
    {
        snake_message = snake_message_2;
    }
    else if(sockfd == psock2)
    {
        snake_message = snake_message_1;
    }
    else
    {
        fprintf(stderr, "[S]: Thread socket error, aborted\n");
        abort();
    }
    send_len = send(sockfd, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
    while(send_len < sizeof(char) * (5 + (64 * 32 * 2)))
    {
        send_len += send(sockfd, snake_message + send_len, sizeof(char) * (5 + (64 * 32 * 2)) - send_len, 0);
    }
    //printf("[S]: %d bytes sent\n", send_len);
    //fflush(stdout);
    send_len = 0;
}







int main(int argc, char **argv)
{
    FILE *fp = fopen("server.log", "w+");
    srand(time(NULL));

    pthread_t recv_thread_1, recv_thread_2, send_thread_1, send_thread_2, send_apple1, send_apple2;

    int sockfd;
    socklen_t clilen;

    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
	abort();
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(DEFAULT_PORT);



    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
	abort();
    }
    listen(sockfd,5);

    //player 1 accept
    clilen = sizeof(cli_addr);
    psock1 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (psock2 < 0)
    {
        perror("ERROR on accept");
	abort();
    }
    printf("[S]: player %d connected. \n", player1);
    fflush(stdout);

    //player 2 accept
    clilen = sizeof(cli_addr);
    psock2 = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (psock2 < 0)
    {
        perror("ERROR on accept");
	abort();
    }
    printf("[S]: player %d connected. \n", player2);
    fflush(stdout);


    //allocate frame
    frame.height = 32;
    frame.width = 64;
    frame.frame = frame_init(frame);


    //allocate memory for snakes
    snake snake1, snake2;
    init_snake(&snake1, frame, 0, 0);
    init_snake(&snake2, frame, 0, 0);
    
    
    //starts game
    write(psock1, "start", 6);
    write(psock2, "start", 6);
    puts("[S]: game started");



    //first apple
    new_apple(&apple, frame);


    //debug variable
    int frame_counter = 1;

    while(1)
    {
        //send apple
        pthread_create(&send_apple1, NULL, send_apple, (void*)(&psock1));
        pthread_create(&send_apple2, NULL, send_apple, (void*)(&psock2));

        pthread_join(send_apple1, NULL);
        pthread_join(send_apple2, NULL);



        //recv
        pthread_create(&recv_thread_1, NULL, recv_from_client, (void*)(&psock1));
        pthread_create(&recv_thread_2, NULL, recv_from_client, (void*)(&psock2));

        pthread_join(recv_thread_1, NULL);
        pthread_join(recv_thread_2, NULL);
        
        c_to_s(&snake1, snake_message_1);
        c_to_s(&snake2, snake_message_2);




        //send snake
        pthread_create(&send_thread_1, NULL, send_to_client, (void*)(&psock1));
        pthread_create(&send_thread_2, NULL, send_to_client, (void*)(&psock2));

        pthread_join(send_thread_1, NULL);
        pthread_join(send_thread_2, NULL);





        //print log
        fprintf(fp, "[frame %d]\n", frame_counter++);

        fprintf(fp, "[apple]: x: %d, y: %d\n", apple.x, apple.y);

        fprintf(fp, "[snake1]: head: x: %d, y: %d\n", snake1.head.x, snake1.head.y);
        fprintf(fp, "[snake1]: direction: %d, length: %d\n", snake1.direction, snake1.length);
        fprintf(fp, "[snake1]: body: ");
        for(int i = 0; i < snake1.length -1; i++)
        {
            fprintf(fp, "%d: x: %d, y: %d     ", i+1, snake1.body[i].x, snake1.body[i].y);
        }
        fprintf(fp, "\n");
        
        fprintf(fp, "[snake2]: head: x: %d, y: %d\n", snake2.head.x, snake2.head.y);
        fprintf(fp, "[snake2]: direction: %d, length: %d\n", snake2.direction, snake2.length);
        fprintf(fp, "[snake2]: body: ");
        for(int i = 0; i < snake2.length -1; i++)
        {
            fprintf(fp, "%d: x: %d, y: %d     ", i+1, snake2.body[i].x, snake2.body[i].y);
        }

        fprintf(fp, "\n\n");






        if(snake1.length < 2 || snake2.length < 2)
        {
            fprintf(stderr, "[S]: packet loss, aborted\n");
            fclose(fp);
            close(sockfd);
            close(psock1);
            close(psock2);
            exit(EXIT_FAILURE);
        }
        if(move_snake(&snake1, frame, &apple) == 1)
        {
            pthread_create(&send_apple2, NULL, send_apple, (void*)(&psock2));
            pthread_join(send_apple2, NULL);

            pthread_create(&send_thread_2, NULL, send_to_client, (void*)(&psock2));
            pthread_join(send_thread_2, NULL);
            
            break;
        }
        if(move_snake(&snake2, frame, &apple) == 1)
        {
            pthread_create(&send_apple1, NULL, send_apple, (void*)(&psock1));
            pthread_join(send_apple1, NULL);

            pthread_create(&send_thread_1, NULL, send_to_client, (void*)(&psock1));
            pthread_join(send_thread_1, NULL);
            
            break;
        }


        
        
        //free message
        free(snake_message_1);
        free(snake_message_2);
        
	
        usleep(1000 / FPS);
    }


    puts("[S]: game ended");







    
    
    //cleanup
    fclose(fp);

    close(sockfd);
    close(psock1);
    close(psock2);
    EXIT_SUCCESS;
}


