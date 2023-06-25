#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "GameRoomHandler.h"

int GameRoomHandler::execute(int sock, unsigned int subOp, RingBuffer& buffer) {
    SetClntIDData setIDData;
    GetGameRoomData getData;
    CreateGameRoomData createData;
    JoinGameRoomData joinData;
    OutGameRoomData outData;

    switch (subOp)
    {
    case HANDLER_GAMEROOM_SETCLNTID:
        if (UnpackData(buffer, setIDData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return SetClntID(sock, setIDData);//에러나면 에러코드가 반환됨.
        break;
    
    case HANDLER_GAMEROOM_GET:
        if (UnpackData(buffer, getData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return GetGameRoom(sock, getData);//에러나면 에러코드가 반환됨.
        break;

    case HANDLER_GAMEROOM_CREATE:
        if (UnpackData(buffer, createData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return CreateGameRoom(sock, createData);//에러나면 에러코드가 반환됨.
        break;
    case HANDLER_GAMEROOM_JOIN:
        if (UnpackData(buffer, joinData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return JoinGameRoom(sock, joinData);//에러나면 에러코드가 반환됨.
        break;

    case HANDLER_GAMEROOM_OUT:
        if (UnpackData(buffer, outData) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] DequeueData() - DataBroken", sock);
            return -1;//Todo: 에러코드로 바꿔주기
        }

        return OutGameRoom(sock, outData);//에러나면 에러코드가 반환됨.
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

int GameRoomHandler::GetGameRoom(int sock, GetGameRoomData& data) {
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(RoomDatas), HANDLER_GAMEROOM, HANDLER_GAMEROOM_GETRETURN, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    RoomDatas returnData = {};
    size_t packet_len = -1;
    std::map<int, GameRoomInfo*>::iterator iter;

    int cnt=0, roomNum=0;
    for (iter = rooms.begin(); iter != rooms.end(); iter++) {
        if ((cnt - data.offset) >= 20)
            break;
        if (cnt >= data.offset) {
            returnData.roomInfo[roomNum] = *iter->second;
            roomNum++;
        }

        cnt++;
    }
    returnData.roomNum = roomNum;

    logger.Log(LOGLEVEL::INFO, "[%s] GetGameRoom - %d", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), roomNum);
    packet_len = GetnerateBasePacket(send_buf, &header, &returnData, &trailer);
    write(sock, send_buf, packet_len);
    return 0;
}

int GameRoomHandler::OutGameRoom(int sock, OutGameRoomData& data) {
    ReturnRoomData returnData = {};
    size_t packet_len = -1;

    GameRoomInfo *roomInfo;
    TCPSOCKETINFO *clntInfo;

    if ((clntInfo = socketManager->getSocketInfo(sock)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%s] OutGameRoom - %d failed: No Info", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), sock);
        return -1;
    }

    if ((roomInfo = FindRoomInfo(data.roomID)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%s] OutGameRoom - %d failed: No Info", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        return -1;
    }

    for (int i=0; i<roomInfo->player_num; i++) {
        if (roomInfo->playerID[i] == clntInfo->id) {
            roomInfo->playerID[i] = 0;
            if (i == 0) {
                roomInfo->playerID[i] = roomInfo->playerID[i+1];
                roomInfo->playerID[i+1] = 0;
            }
            roomInfo->player_num--;
        }
    }
    logger.Log(LOGLEVEL::INFO, "[%s] OutGameRoom: %d", inet_ntoa(clntInfo->sockAddr.sin_addr), clntInfo->id);

    if (roomInfo->player_num <= 0) {
        delete roomInfo;
        roomInfo = NULL;
        rooms.erase(data.roomID);
        logger.Log(LOGLEVEL::INFO, "[%d] DeleteGameRoom", data.roomID);
    }
    
    returnData.isSuccess = 1;
    packet_len = MakeReturnPacket(send_buf, returnData);
    write(sock, send_buf, packet_len);
    
    //Todo: 방이 있을경우에만
    if (roomInfo != NULL && BroadCastRoomInfo(data.roomID) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] Out GameRoom: Broadcast failed", data.roomID);
        return -1;
    }
    return 0;
}

int GameRoomHandler::SetClntID(int sock, SetClntIDData& data) {
    ReturnRoomData returnData = {};
    size_t packet_len = -1;

    TCPSOCKETINFO *clntInfo;

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

int GameRoomHandler::JoinGameRoom(int sock, JoinGameRoomData& data) {
    GameRoomInfo *roomInfo;
    TCPSOCKETINFO *clntInfo;

    if ((roomInfo = FindRoomInfo(data.roomID)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%s] Join GameRoom - %d failed: no room", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        return -1;
    }
    if (roomInfo->player_num >= 2) {
        logger.Log(LOGLEVEL::ERROR, "[%s] Join GameRoom - %d failed: full", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        return -1;
    }
    if ((clntInfo = socketManager->getSocketInfo(sock)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%s] Join GameRoom - %d failed: No ScoketInfo", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        return -1;
    }
    roomInfo->playerID[roomInfo->player_num] = clntInfo->id;
    roomInfo->player_num++;

    if (BroadCastRoomInfo(data.roomID) < 0) {
        logger.Log(LOGLEVEL::ERROR, "[%d] Join GameRoom: Broadcast failed", data.roomID);
        return -1;
    }

    logger.Log(LOGLEVEL::INFO, "[%s] Join GameRoom: %d", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
    return 0;
}

int GameRoomHandler::CreateGameRoom(int sock, CreateGameRoomData& data) {
    GameRoomInfo *info = new GameRoomInfo;
    ReturnRoomData returnData = {};
    size_t packet_len = -1;

    info->roomID = data.roomID;
    info->player_num = 0;

    std::pair<int, GameRoomInfo*> room(data.roomID, info);
    if (!rooms.insert(room).second) {
        logger.Log(LOGLEVEL::ERROR, "[%s] Create GameRoom - %d failed", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);
        delete info;

        return -1;
    }
    logger.Log(LOGLEVEL::INFO, "[%s] Create GameRoom: %d", inet_ntoa(socketManager->getSocketInfo(sock)->sockAddr.sin_addr), data.roomID);

    returnData.isSuccess = 1;
    returnData.roomInfo.roomID = data.roomID;
    packet_len = MakeReturnPacket(send_buf, returnData);
    write(sock, send_buf, packet_len);
    return 0;
}

int GameRoomHandler::BroadCastRoomInfo(int roomID) {
    ReturnRoomData returnData = {};

    GameRoomInfo *roomInfo;
    size_t packet_len = -1;

    if ((roomInfo = FindRoomInfo(roomID)) == NULL) {
        logger.Log(LOGLEVEL::ERROR, "[%d] FindRoomInfo failed: no room", roomID);
        return -1;
    }

    returnData.isSuccess = 1;
    returnData.roomInfo.roomID = roomInfo->roomID;
    returnData.roomInfo.player_num = roomInfo->player_num;
    returnData.roomInfo.playerID[0] = roomInfo->playerID[0];
    returnData.roomInfo.playerID[1] = roomInfo->playerID[1];
    packet_len = MakeReturnPacket(send_buf, returnData);

    for (int i=0; i<roomInfo->player_num; i++) {
        int clnt_sock;

        if ((clnt_sock = socketManager->getSocketByID(roomInfo->playerID[i])) < 0) {
            logger.Log(LOGLEVEL::ERROR, "[%d] getSocketByID failed: no Info", roomInfo->playerID[i]);
            return -1;
        }
        write(clnt_sock, send_buf, packet_len);
    }

    return 0;
}

GameRoomInfo* GameRoomHandler::FindRoomInfo(int roomID) {
    std::map<int, GameRoomInfo*>::iterator iter = rooms.find(roomID);
    if(iter == rooms.end())
        return NULL;

    return iter->second;
}

size_t GameRoomHandler::MakeReturnPacket(void *packet, ReturnRoomData& data) {
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(data), HANDLER_GAMEROOM, HANDLER_GAMEROOM_RETURN, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    return GetnerateBasePacket(packet, &header, &data, &trailer);
}