#ifndef __MULTISNAKE_H__
#define __MULTISNAKE_H__

# ifndef WINVER
# define WINVER 0x0501
# endif
//#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#pragma comment(lib,"ws2_32.lib")


#include "snake.h"
#include "frame.h"
#include "falloc.h"
#include "multiplayer.h"



#define DEFAULT_PORT "50001"
#define BACKUP_PORT "50002"

void multiplayer_hosting();
void multiplayer_join();







#endif