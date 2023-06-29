#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <map>

#include <mutex>

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

    std::recursive_mutex m_mutex;

    void RemoveTCPSOCKETINFO(TCPSOCKETINFO *info);

    // 연결 시 사용
    int AddSocketInfo(int socket);

    int AcceptNewClient();

public:
    SocketManager(int serv_sock, int epfd, JobQueue *jobQueue, const char *saveDir, const char *saveFile) : logger(saveDir, saveFile) {
        this->serv_sock = serv_sock;
        this->epfd = epfd;
        this->jobQueue = jobQueue;
    }

    int getSocketByID(int id);
    TCPSOCKETINFO* getSocketInfo(int socket);
    
    //Todo: id 등록 API 만들기
    // ID 등록 요청 때 사용
    int AddID(int id, int sock);
    // 연결 종료 시 사용
    int DelSocketInfo(int socket);

    int Networking(int event_sock);
};

#endif