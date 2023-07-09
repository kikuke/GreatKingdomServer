#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "UserPacketHandler.h"

int UserPacketHandler::execute(int sock, unsigned int subOp, RingBuffer& buffer) {
    switch (subOp)
    {
    case HANDLER_USER_SETCLNTID:
        return SetClntID(sock, buffer);//에러나면 에러코드가 반환됨.
        break;

    case HANDLER_USER_ECHOTEST:
        return EchoMessage(sock, buffer);//에러나면 에러코드가 반환됨.
        break;
    
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] execute() - DataBroken", sock);
        return 0;
        break;
    }
    return 1;
}

int UserPacketHandler::catchError(int sock, unsigned int errorCode) {
    ReturnUserData returnData = {0};
    size_t packet_len = -1;

    switch (errorCode)
    {
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] catchError - Undefined ErrorCode: %d", sock, errorCode);
        packet_len = MakeReturnPacket(send_buf, returnData);
        write(sock, send_buf, packet_len);
        return -1;
        break;
    }
    return 0;
}

int UserPacketHandler::SetClntID(int sock, RingBuffer& buffer) {
    ReturnUserData returnData = {};
    size_t packet_len = -1;

    TCPSOCKETINFO *clntInfo;

    SetClntIDData data;

    if (UnpackData(buffer, &data) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
        return -1;//Todo: 에러코드로 바꿔주기
    }

    if ((clntInfo = socketManager->getSocketInfo(sock)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%s] AddID - %d failed: No Info", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.clnt_id);
        return -1;
    }

    if (socketManager->AddID(data.clnt_id, sock) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%s] AddID - %d failed: id conflict", inet_ntoa(clntInfo->sockAddr.sin_addr), data.clnt_id);
        return -1;
    }
    logger.Log(LOGLEVEL::INFO, "[%s] SetClntID: %d", inet_ntoa(clntInfo->sockAddr.sin_addr), data.clnt_id);

    returnData.isSuccess = 1;
    packet_len = MakeReturnPacket(send_buf, returnData);
    write(sock, send_buf, packet_len);
    return 0;
}

int UserPacketHandler::EchoMessage(int sock, RingBuffer& buffer) {
    EchoData data;

    if (UnpackData(buffer, &data) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
        return -1;//Todo: 에러코드로 바꿔주기
    }

    if (write(sock, data.msg, sizeof(data.msg)) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] write error: %s", sock, strerror(errno));
        return -1;//Todo: 에러코드로 바꿔주기
    }
    logger.Log(LOGLEVEL::INFO, "[%d] EchoMessage: %s", sock, data.msg);

    return 0;
}

size_t UserPacketHandler::MakeReturnPacket(void *packet, ReturnUserData& data) {
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(data), HANDLER_USER, HANDLER_GAMEROOM_RETURN, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    return GetnerateBasePacket(packet, &header, &data, &trailer);
}