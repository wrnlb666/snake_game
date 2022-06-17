#include "multisnake.h"



void start_game(struct frame frame)
{
    set_blank_frame(frame);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) -7, "starts in 3...", 14);
    print_screen(frame);
    sleep(1);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) -7, "starts in 2...", 14);
    print_screen(frame);
    sleep(1);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) -7, "starts in 1...", 14);
    print_screen(frame);
    sleep(1);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) -7, "starts in 0...", 14);
    print_screen(frame);
    sleep(1);
}






void game_over(int player, struct frame frame)
{
    set_blank_frame(frame);
    switch(player)
    {
        case host:
        {
            strncpy(frame.frame[frame.height /2] + (frame.width /2) - 9, "You Lost! Sucker!", 17);
            break;
        }
        case guest:
        {
            strncpy(frame.frame[frame.height /2] + (frame.width /2) - 9, "You Lost! Sucker!", 17);
            break;
        }
        case tie:
        {
            strncpy(frame.frame[frame.height /2] + (frame.width /2) - 16, "What an Epic Fight! It's a Tie!", 31);
            break;
        }
    }
    print_screen(frame);
    sleep(3);
}



void win(struct frame frame)
{
    set_blank_frame(frame);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) - 11, "You are the Champion!", 21);
    print_screen(frame);
    sleep(3);
}






void multiplayer_hosting()
{
    srand(time(NULL));
    struct frame frame;
    frame.height = 32;
    frame.width = 64;

    frame.frame = frame_init(frame);

    clear_screen();
    set_blank_frame(frame);

    strncpy(frame.frame[frame.height /2] + (frame.width /2) - 10, "Waiting For Guest...", 20);
    print_screen(frame);


    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) 
    {
        printf("WSAStartup failed: %d\n", iResult);
        exit(EXIT_FAILURE);
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) 
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    SOCKET ListenSocket = INVALID_SOCKET;

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(ListenSocket == INVALID_SOCKET) 
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR) 
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    //listening on sockets
    if(listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR) 
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //Accept connection
    SOCKET ClientSocket;

    ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if(ClientSocket == INVALID_SOCKET) 
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //close listen socket
    closesocket(ListenSocket);

    //successfully connected
    set_blank_frame(frame);
    strncpy(frame.frame[frame.height /3] + (frame.width /2) - 8, "Guest Connected!", 16);
    strncpy(frame.frame[(frame.height /3) *2] + (frame.width /2) - 10, "Press ENTER to Start!", 21);
    print_screen(frame);

    while(1)
    {
        if(kbhit())
        {
            char ch = getch();
            if(ch == 13)
            {
                char *start_buffer = "start";
                if(send(ClientSocket, start_buffer, 6, 0) == SOCKET_ERROR)
                {
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
    }

    start_game(frame);


    //Finally starts to play
    snake snake_host;
    snake snake_guest;

    init_snake(&snake_host, frame, (2 + rand()%(frame.height -4)), (2 + rand()%(frame.width -4)));
    init_snake(&snake_guest, frame, 0, 0);
    position guest_head;

    
    position apple;
    char *apple_buf;
    apple.x = apple.y = 0;
    new_apple(&apple, frame);
    check_apple(&apple, snake_host, snake_guest, frame);

    char *snake_message;

    clear_screen();
    set_blank_frame(frame);

    int send_counter, recv_counter;
    send_counter = recv_counter = 0;

    while(1)
    {
        //send apple position
        apple_buf = p_to_c(apple);
        send(ClientSocket, apple_buf, 2, 0);
        free(apple_buf);
        render_apple(apple, frame);


        //send snake_host status
        snake_message = s_to_c(snake_host);
        send_counter = send(ClientSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
        while(send_counter < sizeof(char) * (5 + (64 * 32 * 2)))
        {
            send_counter += send(ClientSocket, snake_message + send_counter, sizeof(char) * (5 + (64 * 32 * 2)) - send_counter, 0);
        }
        send_counter = 0;
        render_snake(snake_host, frame, '#');


        //receive snake_guest status
        guest_head = snake_guest.head;
        ZeroMemory(snake_message, sizeof(char) * (5 + (64 * 32 * 2)));
        recv_counter = recv(ClientSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
        while(recv_counter < sizeof(char) * (5 + (64 * 32 * 2)))
        {
            recv_counter += recv(ClientSocket, snake_message + recv_counter, sizeof(char) * (5 + (64 * 32 * 2)) - recv_counter, 0);
        }
        recv_counter = 0;
        c_to_s(&snake_guest, snake_message);
        render_snake(snake_guest, frame, 'O');


        //clean up
        free(snake_message);


        //print screen
        print_screen(frame);
        Sleep(1000 /FPS);



        //fetching user inputs
        if(kbhit())
        {
            char ch = getch();
            switch(ch)
            {
                case (27):
                    game_over(host, frame);
                    goto host_cleanup;
                    break;

                case ('w'):
                {
                    if(snake_host.direction != down && snake_host.direction != up)
                    {
                        snake_host.direction = up;
                    }
                    break;
                }
                case ('s'):
                {
                    if(snake_host.direction != up && snake_host.direction != down)
                    {
                        snake_host.direction = down;
                    }
                    break;
                }                
                case ('a'):
                {
                    if(snake_host.direction != right && snake_host.direction != left)
                    {
                        snake_host.direction = left;
                    }
                    break;
                }
                case ('d'):
                {
                    if(snake_host.direction != left && snake_host.direction != right)
                    {
                        snake_host.direction = right;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        if(move_snake(&snake_host, frame, &apple))
        {
            //send apple position
            apple_buf = p_to_c(apple);
            send(ClientSocket, apple_buf, 2, 0);
            free(apple_buf);
            render_apple(apple, frame);

            //send snake_host status
            snake_message = s_to_c(snake_host);
            send(ClientSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
            render_snake(snake_host, frame, '#');

            free(snake_message);

            game_over(host, frame);

            goto host_cleanup;
        }


        if(check_snake(frame, snake_host, snake_guest) == host)
        {
            //send apple position
            apple_buf = p_to_c(apple);
            send(ClientSocket, apple_buf, 2, 0);
            free(apple_buf);

            //send snake_host status
            snake_message = s_to_c(snake_host);
            send(ClientSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
            free(snake_message);

            game_over(host, frame);

            goto host_cleanup;
        }
        else if(check_snake(frame, snake_host, snake_guest) == guest)
        {
            win(frame);
            goto host_cleanup;
        }
        else if(check_snake(frame, snake_host, snake_guest) == tie)
        {
            game_over(tie, frame);
            goto host_cleanup;
        }




        
        set_blank_frame(frame);

    }
    host_cleanup:






    //Disconnect
    //shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) 
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    //cleanup
    closesocket(ClientSocket);
    WSACleanup();
}








void multiplayer_join()
{
    struct frame frame;
    frame.height = 32;
    frame.width = 64;

    frame.frame = frame_init(frame);
    
    char *IP_address, *IP_buffer;

    enter_IP:

    clear_screen();
    set_blank_frame(frame);

    strncpy(frame.frame[frame.height /3] + (frame.width /2) - 13, "Please Enter Host's IP...", 25);
    print_screen(frame);


    IP_buffer = malloc(sizeof(char) * 20);
    ZeroMemory(IP_buffer, sizeof(char) * 20);
    int buffer_position = 0;
    
    while(1)
    {
        char ch = getch();

        //number && dot
        if(ch == '1' || ch == '2' || ch == '3' || ch == '4'|| ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9' || ch == '0' || ch == '.')
        {
            IP_buffer[buffer_position++] = ch;
        }
        //back space
        else if(ch == 8)
        {
            if(buffer_position != 0)
            {
                IP_buffer[--buffer_position] = 0;
                set_blank_frame(frame);
                strncpy(frame.frame[frame.height /3] + (frame.width /2) - 13, "Please Enter Host's IP...", 25);
            }
        }
        //enter
        else if(ch == 13)
        {
            IP_buffer[buffer_position] = 0;
            break;
        }
        //ESC
        else if(ch == 27)
        {
            exit(EXIT_SUCCESS);
        }
        
        //heap overflow detection
        if(buffer_position >= 16)
        {
            set_blank_frame(frame);
            strncpy(frame.frame[frame.height /3] + (frame.width /2) - 13, "Please Enter Host's IP...", 25);
            strncpy(frame.frame[(frame.height /3) *2] + (frame.width /2) - 13, "Wrong Format!", 13);
            ZeroMemory(IP_buffer, sizeof(char)*20);
            buffer_position = 0;
            print_screen(frame);
            sleep(3);
            set_blank_frame(frame);
            strncpy(frame.frame[frame.height /3] + (frame.width /2) - 13, "Please Enter Host's IP...", 25);
        }
        
        //print screen
        strncpy(frame.frame[(frame.height /3) *2] + (frame.width /2) - 13, IP_buffer, buffer_position);
        print_screen(frame);
    }

    IP_address = malloc(sizeof(char) * (buffer_position +1));
    strncpy(IP_address, IP_buffer, (buffer_position +1));
    free(IP_buffer);


    
    WSADATA wsaData;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
    {
        printf("WSAStartup failed: %d\n", iResult);
        exit(EXIT_FAILURE);
    }


    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(IP_address, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) 
    {
        iResult = getaddrinfo(IP_address, BACKUP_PORT, &hints, &result);
        if(iResult != 0)
        {
            //printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            set_blank_frame(frame);
            strncpy(frame.frame[frame.height /2] + (frame.width /2) - 9, "Wrong IP Address!", 17);
            print_screen(frame);
            sleep(3);
            goto enter_IP;
        }
    }
    free(IP_address);

    SOCKET ConnectSocket = INVALID_SOCKET;


    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);


    if(ConnectSocket == INVALID_SOCKET) 
    {
        //printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        set_blank_frame(frame);
        strncpy(frame.frame[frame.height /2] + (frame.width /2) - 7, "Socket Error!", 13);
        print_screen(frame);
        sleep(3);
        goto enter_IP;
    }





    // Connect to server.
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if(iResult == SOCKET_ERROR) 
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);
    if(ConnectSocket == INVALID_SOCKET) 
    {
        WSACleanup();
        set_blank_frame(frame);
        strncpy(frame.frame[frame.height /2] + (frame.width /2) - 9, "Wrong IP Address!", 17);
        print_screen(frame);
        sleep(3);
        goto enter_IP;
    }



    //successfully connected
    set_blank_frame(frame);
    strncpy(frame.frame[frame.height /2] + (frame.width /2) - 15, "Waiting For Host to Start...", 28);
    print_screen(frame);


    char *game_start = falloc(sizeof(char) * 64);
    while(1)
    {
        if(recv(ConnectSocket, game_start, 64, 0))
        {
            if(strncmp(game_start, "start", 6) == 0)
            {
                break;
            }
            fprintf(stderr, "%s", game_start);
        }
    }


    start_game(frame);




    //Finally starts to play
    snake snake_host;
    snake snake_guest;

    init_snake(&snake_host, frame, 0, 0);
    init_snake(&snake_guest, frame, (2 + rand()%(frame.height -4)), (2 + rand()%(frame.width -4)));
    position host_head;
    
    position apple;
    char *apple_buf = malloc(sizeof(char) *2);
    apple.x = apple.y = 0;

    char *snake_buf = malloc(sizeof(char) * (5 + (64 * 32 * 2)));
    char *snake_message;

    clear_screen();
    set_blank_frame(frame);

    int send_counter, recv_counter;
    send_counter = recv_counter = 0;

    while(1)
    {
        //recv apple position
        recv(ConnectSocket, apple_buf, 2, 0);
        apple = c_to_p(apple_buf);
        render_apple(apple, frame);



        //send snake_guest status
        snake_message = s_to_c(snake_guest);
        send_counter = send(ConnectSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
        while(send_counter < sizeof(char) * (5 + (64 * 32 * 2)))
        {
            send_counter += send(ConnectSocket, snake_message + send_counter, sizeof(char) * (5 + (64 * 32 * 2)) - send_counter, 0);
        }
        send_counter = 0;
        free(snake_message);
        render_snake(snake_guest, frame, '#');



        //recv snake_host status
        host_head = snake_host.head;
        recv_counter = recv(ConnectSocket, snake_buf, sizeof(char) * (5 + (64 * 32 * 2)), 0);
        while(recv_counter < sizeof(char) * (5 + (64 * 32 * 2)))
        {
            recv_counter += recv(ConnectSocket, snake_buf + recv_counter, sizeof(char) * (5 + (64 * 32 * 2)) - recv_counter, 0);
        }
        recv_counter = 0;
        c_to_s(&snake_host, snake_buf);
        render_snake(snake_host, frame, 'O');



        //print screen
        print_screen(frame);
        Sleep(1000 /FPS);




        //fetching user inputs
        if(kbhit())
        {
            char ch = getch();
            switch(ch)
            {
                case ('w'):
                {
                    if(snake_guest.direction != down && snake_guest.direction != up)
                    {
                        snake_guest.direction = up;
                    }
                    break;
                }
                case ('s'):
                {
                    if(snake_guest.direction != up && snake_guest.direction != down)
                    {
                        snake_guest.direction = down;
                    }
                    break;
                }                
                case ('a'):
                {
                    if(snake_guest.direction != right && snake_guest.direction != left)
                    {
                        snake_guest.direction = left;
                    }
                    break;
                }
                case ('d'):
                {
                    if(snake_guest.direction != left && snake_guest.direction != right)
                    {
                        snake_guest.direction = right;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        
        if(move_snake(&snake_guest, frame, &apple))
        {
            //send snake_guest status
            snake_message = s_to_c(snake_guest);
            send(ConnectSocket, snake_message, sizeof(char) * (5 + (64 * 32 * 2)), 0);
            free(snake_message);

            game_over(guest, frame);

            goto guest_cleanup;
        }
        if(check_snake(frame, snake_host, snake_guest) == guest)
        {
            game_over(guest, frame);
            goto guest_cleanup;
        }
        else if(check_snake(frame, snake_host, snake_guest) == tie)
        {
            game_over(tie, frame);
            goto guest_cleanup;
        }
        else if(check_snake(frame, snake_host, snake_guest) == host)
        {
            win(frame);
            goto guest_cleanup;
        }


        
        //game already ended at the host side
        if(host_head.x == snake_host.head.x && host_head.y == snake_host.head.y)
        {
            win(frame);
            goto guest_cleanup;
        }
        
        

        set_blank_frame(frame);


    }
    win(frame);
    guest_cleanup:




    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}














