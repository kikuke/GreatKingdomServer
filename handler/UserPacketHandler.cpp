#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "UserPacketHandler.h"

int UserPacketHandler::execute(int sock, unsigned int subOp, RingBuffer& buffer) {
    EchoData data;
    switch (subOp)
    {
    case HANDLER_USER_ECHOTEST:
        if (UnpackData(buffer, &data) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return EchoMessage(sock, data);//에러나면 에러코드가 반환됨.
        break;
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] execute() - DataBroken", sock);
        return 0;
        break;
    }
    return 1;
}

int UserPacketHandler::catchError(int sock, unsigned int errorCode) {
    switch (errorCode)
    {
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] catchError - Undefined ErrorCode: %d", sock, errorCode);
        return -1;
        break;
    }
    return 0;
}

int UserPacketHandler::EchoMessage(int sock, EchoData& echo) {
    if (write(sock, echo.msg, sizeof(echo.msg)) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] write error: %s", sock, strerror(errno));
        return -1;//Todo: 에러코드로 바꿔주기
    }
    logger.Log(LOGLEVEL::INFO, "[%d] EchoMessage: %s", sock, echo.msg);

    return 0;
}