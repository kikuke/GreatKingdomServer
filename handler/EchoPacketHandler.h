#ifndef ECHO_PACKET_HANDLER_H
#define ECHO_PACKET_HANDLER_H

#include "ServerInfo.h"
#include "Logger.h"
#include "BasePacketHandler.h"

class EchoPacketHandler : public BasePacketHandler
{
private:

    Logger logger;
public:
    EchoPacketHandler(const char *saveDir, const char *saveFile) : BasePacketHandler(HANDLER_ECHO), logger(saveDir, saveFile) {
        
    }

    int execute(int sock, unsigned int subOp, RingBuffer& buffer) override;
    int catchError(int sock, unsigned int errorCode) override;
};

#endif