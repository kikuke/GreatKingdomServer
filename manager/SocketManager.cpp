#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>

#include "epoll.h"
#include "SocketManager.h"

int SocketManager::getSocketByID(int id) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    std::map<int, int>::iterator iter = id_socketMap.find(id);
    if(iter == id_socketMap.end())
        return -1;

    return iter->second;
}

TCPSOCKETINFO* SocketManager::getSocketInfo(int socket) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    std::map<int, TCPSOCKETINFO*>::iterator iter = socketInfo.find(socket);
    if(iter == socketInfo.end())
        return NULL;

    return iter->second;
}

int SocketManager::AddID(int id, int sock) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    TCPSOCKETINFO *info;

    if (!id_socketMap.insert(std::pair<int, int>(id, sock)).second) {
        logger.Log(LOGLEVEL::ERROR, "Insert ID conflict!: %d", id);
        return -1;
    }

    if ((info = getSocketInfo(sock)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "No SocketInfo: %d", sock);

        id_socketMap.erase(id);
        return -1;
    }
    info->id = id;

    logger.Log(LOGLEVEL::INFO, "AddID - ID: %d, Socket: %d", id, sock);
    return 0;
}

void SocketManager::RemoveTCPSOCKETINFO(TCPSOCKETINFO *info) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    delete info->recvBuffer;
    delete info;
}

int SocketManager::AddSocketInfo(int socket) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    TCPSOCKETINFO *info = new TCPSOCKETINFO;
    socklen_t socklen = sizeof(info->sockAddr);

    //id는 이 단계에선 없는 상태
    info->id = -1;
    info->socket = socket;
    info->recvBuffer = new TSRingBuffer();
    getpeername(socket, (struct sockaddr*)&(info->sockAddr), &socklen);

    if (!socketInfo.insert(std::pair<int, TCPSOCKETINFO*>(socket, info)).second) {
        logger.Log(LOGLEVEL::ERROR, "Insert Socket Info conflict!: %d", socket);
        RemoveTCPSOCKETINFO(info);
        return -1;
    }

    logger.Log(LOGLEVEL::DEBUG, "Add Socket: %d", socket);
    return 0;
}

int SocketManager::DelSocketInfo(int socket) {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    TCPSOCKETINFO *info;

    if ((info = getSocketInfo(socket)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "No SocketInfo: %d", socket);
        return -1;
    }

    if ((info->id != -1) && (getSocketByID(info->id) > 0)) {
        logger.Log(LOGLEVEL::DEBUG, "Delete ID: %d", info->id);
        id_socketMap.erase(info->id);
    }
    RemoveTCPSOCKETINFO(info);
    socketInfo.erase(socket);

    logger.Log(LOGLEVEL::DEBUG, "Delete Socket: %d", socket);
    return 0;
}

int SocketManager::AcceptNewClient() {
    std::unique_lock<std::recursive_mutex> lock(m_mutex);

    int clnt_sock = -1;
    struct sockaddr_in clnt_addr;
    socklen_t addr_sz = sizeof(clnt_addr);

    if ((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz)) < 0) {
        logger.Log(LOGLEVEL::ERROR, "accept failed: %s", strerror(errno));
        return -1;
    }

    if (AddETClntSock(epfd, clnt_sock) < 0) {
        logger.Log(LOGLEVEL::ERROR, "AddETClntSock failed: %s", strerror(errno));
        close(clnt_sock);
        return -1;
    }

    if (AddSocketInfo(clnt_sock) < 0) {
        logger.Log(LOGLEVEL::ERROR, "AddSocketInfo failed: %s", strerror(errno));
        epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_sock, NULL);
        close(clnt_sock);
        return -1;
    }

    logger.Log(LOGLEVEL::INFO, "[%s] Connecting Server", inet_ntoa(clnt_addr.sin_addr));
    return 0;
}

int SocketManager::Networking(int event_sock) {
    //case: accept
    if (event_sock == serv_sock) {
        if (AcceptNewClient()) {
            logger.Log(LOGLEVEL::ERROR, "AcceptNewClient Error: %s", strerror(errno));
            return -1;
        }

        logger.Log(LOGLEVEL::DEBUG, "Accept Socket: %d", event_sock);
        return 0;
    }

    jobQueue->readQueue.push(event_sock);
    logger.Log(LOGLEVEL::DEBUG, "Get Data Socket: %d", event_sock);
    return 0;
}