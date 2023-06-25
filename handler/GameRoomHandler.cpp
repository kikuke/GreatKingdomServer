#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "GameRoomHandler.h"

int GameRoomHandler::execute(int sock, unsigned int subOp, RingBuffer& buffer) {
    CreateGameRoomData createData;

    switch (subOp)
    {
    case HANDLER_GAMEROOM_CREATE:
        if (UnpackData(buffer, createData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return CreateGameRoom(sock, createData);//에러나면 에러코드가 반환됨.
        break;
    default:
        logger.Log(LOGLEVEL::ERROR, "[%d] execute() - DataBroken", sock);
        return 0;
        break;
    }
    return 1;
}

int GameRoomHandler::catchError(int sock, unsigned int errorCode) {
    ReturnRoomData returnData = {0};
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


int GameRoomHandler::CreateGameRoom(int sock, CreateGameRoomData& data) {
    GameRoomInfo *info = new GameRoomInfo;
    ReturnRoomData returnData;
    size_t packet_len = -1;

    info->roomID = data.roomID;

    std::pair<int, GameRoomInfo*> room(data.roomID, info);
    if (!rooms.insert(room).second) {
        logger.Log(LOGLEVEL::INFO, "[%s] Create GameRoom - %d failed", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        delete info;

        return -1;
    }
    logger.Log(LOGLEVEL::INFO, "[%s] Create GameRoom: %d", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);

    returnData.isSuccess = 1;
    returnData.roomID = rooms.find(data.roomID)->second->roomID;
    packet_len = MakeReturnPacket(send_buf, returnData);
    write(sock, send_buf, packet_len);
    return 0;
}

size_t GameRoomHandler::MakeReturnPacket(void *packet, ReturnRoomData& data) {
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(data), HANDLER_GAMEROOM, HANDLER_GAMEROOM_RETURN, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    return GetnerateBasePacket(packet, &header, &data, &trailer);
}