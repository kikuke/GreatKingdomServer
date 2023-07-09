#ifndef ECHO_PACKET_HANDLER_H
#define ECHO_PACKET_HANDLER_H

#include "ServerInfo.h"
#include "Logger.h"
#include "BasePacketHandler.h"

class UserPacketHandler : public BasePacketHandler
{
private:

    Logger logger;

    int EchoMessage(int sock, EchoData& echo);
public:
    UserPacketHandler(const char *saveDir, const char *saveFile) : BasePacketHandler(HANDLER_USER), logger(saveDir, saveFile) {
        
    }

    int execute(int sock, unsigned int subOp, RingBuffer& buffer) override;
    int catchError(int sock, unsigned int errorCode) override;
};

#endif