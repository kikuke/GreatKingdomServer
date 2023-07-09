#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <netinet/in.h>

#include "TSRingBuffer.h"
#include "TSQueue.h"

#define HANDLER_USER 0x01
struct ReturnUserData
{
    // if success 1, else 0
    int isSuccess;
};

#define HANDLER_USER_SETCLNTID 0x00
struct SetClntIDData
{
    int clnt_id;
};

#define HANDLER_USER_ECHOTEST 0xFF
#define ECHO_MAX_SIZE 100

#pragma pack(push, 4)

struct GameRoomInfo
{
    int roomID;
    int player_num;
    int playerID[2];

    int roomStatus;

    int nowTurnID;
    int panel[2*9*9];
    double score[2];
    int isPass[2];
};

#pragma pack(pop)

struct EchoData
{
    char msg[ECHO_MAX_SIZE];
};

#define HANDLER_GAMEROOM 0x02
struct ReturnRoomData
{
    // if success 1, else 0
    int isSuccess;

    GameRoomInfo roomInfo;
};

#define HANDLER_GAMEROOM_RETURN 0x01
#define HANDLER_GAMEROOM_GET 0x02
#define HANDLER_GAMEROOM_GETRETURN 0x0F
struct GetGameRoomData
{
    int offset;
};

struct RoomDatas
{
    int roomNum;

    GameRoomInfo roomInfo[20];
};

#define HANDLER_GAMEROOM_CREATE 0x03
struct CreateGameRoomData
{
    int roomID;
};

#define HANDLER_GAMEROOM_JOIN 0x04
struct JoinGameRoomData
{
    int roomID;
};

#define HANDLER_GAMEROOM_OUT 0x05
struct OutGameRoomData
{
    int roomID;
};

#define HANDLER_GAMEROOM_UPDATE 0x06
struct UpdateGameRoomData
{
    GameRoomInfo roomInfo;
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