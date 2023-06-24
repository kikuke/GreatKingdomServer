#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <netinet/in.h>

#include "TSRingBuffer.h"
#include "TSQueue.h"

#define HANDLER_ECHO 0x01
#define HANDLER_ECHO_ECHOTEST 0x01
#define ECHO_MAX_SIZE 100

struct EchoData
{
    char msg[ECHO_MAX_SIZE];
};

#define HANDLER_GAMEROOM 0x01
struct ReturnRoomData
{
    // if success 1, else 0
    int isSuccess;

};

#define HANDLER_GAMEROOM_RETURN 0x00
#define HANDLER_GAMEROOM_GET 0x01

#define HANDLER_GAMEROOM_CREATE 0x02
struct CreateGameRoomData
{
    int roomID;
};

#define HANDLER_GAMEROOM_ENTER 0x03
#define HANDLER_GAMEROOM_DELETE 0x04

struct GameRoomInfo
{
    int roomID;
    int player_num;
    int player_socket[2];
};

struct JobQueue
{
    TSQueue<int> readQueue;
    TSQueue<int> workQueue;
    TSQueue<int> broadcastQueue;
};

struct TCPSOCKETINFO
{
    // client_id
    int id;

    int socket;
    struct sockaddr_in sockAddr;

    TSRingBuffer* recvBuffer;
};

#endif