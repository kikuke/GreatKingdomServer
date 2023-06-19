#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <netinet/in.h>

#include "TSRingBuffer.h"
#include "TSQueue.h"

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