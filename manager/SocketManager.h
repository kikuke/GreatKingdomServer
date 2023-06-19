#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <map>

#include "ServerInfo.h"
#include "Logger.h"

class SocketManager
{
private:
    int serv_sock;
    int epfd;
    JobQueue *jobQueue;

    std::map<int, int> id_socketMap;
    std::map<int, TCPSOCKETINFO*> socketInfo;

    Logger logger;

    void RemoveTCPSOCKETINFO(TCPSOCKETINFO *info);

    int AddID(int id, int sock);

    int AddSocketInfo(int socket);
    int DelSocketInfo(int socket);

    int AcceptNewClient();

public:
    SocketManager(int serv_sock, int epfd, JobQueue *jobQueue, const char *saveDir, const char *saveFile) : logger(saveDir, saveFile) {
        this->serv_sock = serv_sock;
        this->epfd = epfd;
        this->jobQueue = jobQueue;
    }

    int getSocketByID(int id);
    TCPSOCKETINFO* getSocketInfo(int socket);

    int Networking(int event_sock);
};

#endif