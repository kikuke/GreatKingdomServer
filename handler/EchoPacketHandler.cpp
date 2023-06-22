#include "EchoPacketHandler.h"

int EchoPacketHandler::execute(int sock, unsigned int subOp, RingBuffer& buffer) {
    DisconnectData data;
    switch (subOp)
    {
    case HANDLER_ECHO_ECHOTEST:
        if(DequeueData(data, buffer) != TCP_PACKET_END_CODE){
            logger.Log(LOGLEVEL::ERROR, "[%s] DequeueData() - DataBroken", sock);
            return 0;//Todo: 에러코드로 바꿔주기
        }

        return Disconnect(sock, data);//에러나면 에러코드가 반환됨.
        break;
    default:
        logger.Log(LOGLEVEL::ERROR, "[%s] execute() - DataBroken", sock);
        return 0;
        break;
    }
    return 1;
}

int EchoPacketHandler::catchError(int sock, unsigned int errorCode) {
    switch (errorCode)
    {
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] catchError - Undefined ErrorCode: %d", sock, errorCode);
        return -1;
        break;
    }
    return 0;
}