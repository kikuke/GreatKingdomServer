#ifndef ECHO_PACKET_HANDLER_H
#define ECHO_PACKET_HANDLER_H

#include "ServerInfo.h"
#include "Logger.h"
#include "BasePacketHandler.h"
#include "SocketManager.h"

#define MAX_BUFFER_SIZE 2048*2*2*2

class UserPacketHandler : public BasePacketHandler
{
private:
    SocketManager *socketManager;
    
    char send_buf[MAX_BUFFER_SIZE];

    Logger logger;

    //return packet length
    size_t MakeReturnPacket(void *packet, ReturnUserData& data);

    //* handler *

    int EchoMessage(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int SetClntID(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int CloseUser(int sock, RingBuffer& buffer);

public:
    UserPacketHandler(const char *saveDir, const char *saveFile, SocketManager *socketManager) : BasePacketHandler(HANDLER_USER), logger(saveDir, saveFile) {
        this->socketManager = socketManager;
    }

    int execute(int sock, unsigned int subOp, RingBuffer& buffer) override;
    int catchError(int sock, unsigned int errorCode) override;
};

#endif