#ifndef SERVER_THREAD_H
#define SERVER_THREAD_H

#include "ServerInfo.h"

ssize_t RingBufferReader(int fd, void* buf, size_t buf_sz, RingBuffer *ringBuffer);
void ReadThread(SocketManager* socketManager, JobQueue* jobQueue, const int buf_sz);

#endif