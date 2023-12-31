#ifndef GAMEROOM_HANDLER_H
#define GAMEROOM_HANDLER_H

#include <map>

#include "ServerInfo.h"
#include "Logger.h"
#include "BasePacketHandler.h"
#include "SocketManager.h"

#define MAX_BUFFER_SIZE 2048*2*2*2

class GameRoomHandler : public BasePacketHandler
{
private:
    SocketManager *socketManager;
    std::map<int, GameRoomInfo*> rooms;
    
    char send_buf[MAX_BUFFER_SIZE];

    Logger logger;

    //if success return 0, error return -1
    int InitGameRoomInfo(GameRoomInfo *info);

    //success return info else NULL
    GameRoomInfo* FindRoomInfo(int roomID);

    //return packet length
    size_t MakeReturnPacket(void *packet, ReturnRoomData& data);

    //if success return 0, error return -1
    int UpdateInfo(GameRoomInfo *info, GameRoomInfo data);

    //if success return 0, error return -1
    int BroadCastRoomInfo(int roomID);

    //* Handler *
    //if success return 0, error return -1
    int GetGameRoom(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int CreateGameRoom(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int JoinGameRoom(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int OutGameRoom(int sock, RingBuffer& buffer);

    //if success return 0, error return -1
    int UpdateGameRoom(int sock, RingBuffer& buffer);
public:
    GameRoomHandler(const char *saveDir, const char *saveFile, SocketManager *socketManager) : BasePacketHandler(HANDLER_GAMEROOM), logger(saveDir, saveFile) {
        this->socketManager = socketManager;
    }

    int execute(int sock, unsigned int subOp, RingBuffer& buffer) override;
    int catchError(int sock, unsigned int errorCode) override;
};

#endif