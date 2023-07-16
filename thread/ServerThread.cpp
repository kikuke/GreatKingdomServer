#include <unistd.h>
#include <arpa/inet.h>

#include "SocketManager.h"
#include "Logger.h"
#include "ServerThread.h"

#define LOG_DIR "../logfile"

ssize_t RingBufferReader(int fd, void *buf, size_t buf_sz, RingBuffer *ringBuffer) {
    ssize_t temp_len = -1;
    ssize_t str_len = 0;

    while(1)
    {
        temp_len = read(fd, buf,  buf_sz);
        if(temp_len == 0){
            return 0;
        }
        else if(temp_len < 0)
        {
            if(errno == EAGAIN)
                return str_len;
            else
                return -1;
        }
        else
        {
            str_len += ringBuffer->enqueue(buf, temp_len);
        }
    }
}

void ReadThread(SocketManager *socketManager, JobQueue *jobQueue, const int buf_sz) {
    int sock;

    char buf[buf_sz];
    TCPSOCKETINFO *info;

    Logger logger(LOG_DIR, "ReadThread.txt");
    logger.Log(LOGLEVEL::DEBUG, "Read Thread Start...");

    while ((sock = jobQueue->readQueue.pop())) {
        logger.Log(LOGLEVEL::DEBUG, "Read Socket: %d", sock);

        if ((info = socketManager->getSocketInfo(sock)) == NULL) {
            //아마 접속 종료 완료 후에 확인 차 발생
            logger.Log(LOGLEVEL::ERROR, "Can't find Socket To Read: %d", sock);
            continue;
        }
        //disconnect
        if (RingBufferReader(sock, buf, buf_sz, info->recvBuffer) == 0) {
            logger.Log(LOGLEVEL::DEBUG, "[%s]Disconnecting", inet_ntoa((info->sockAddr).sin_addr));

            continue;
        }

        jobQueue->workQueue.push(sock);
    }

    logger.Log(LOGLEVEL::ERROR, "Read Thread Down...");
}

void WorkThread(SocketManager *socketManager, JobQueue *jobQueue, BasePacketManager *basePacketManager) {
    int sock;

    Logger logger(LOG_DIR, "WorkThread.txt");
    logger.Log(LOGLEVEL::DEBUG, "Work Thread Start...");

    while ((sock = jobQueue->workQueue.pop())) {
        TCPSOCKETINFO *info = nullptr;
        logger.Log(LOGLEVEL::DEBUG, "Work Socket: %d", sock);

        if ((info = socketManager->getSocketInfo(sock)) == NULL) {
            logger.Log(LOGLEVEL::ERROR, "Can't find Socket To Work: %d", sock);
            continue;
        }

        while (basePacketManager->execute(sock, *(info->recvBuffer)) != -1) {
            logger.Log(LOGLEVEL::DEBUG, "basePacketManager execute: %d", sock);
        }
    }

    logger.Log(LOGLEVEL::ERROR, "Work Thread Down...");
}