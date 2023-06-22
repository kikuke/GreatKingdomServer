#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <netinet/in.h>

#include "TSRingBuffer.h"
#include "TSQueue.h"

#define HANDLER_ECHO 0x01
#define HANDLER_ECHO_ECHOTEST 0x01

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