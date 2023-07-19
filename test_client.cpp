#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "BasePacket.h"
#include "BasePacketDefine.h"
#include "ServerInfo.h"

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 1234
#define BUFFER_MAX_SIZE 2048

int HeaderCheck(BasePacketHeader& header);
int TrailerCheck(BasePacketTrailer& trailer);
template <typename T>
int UnpackData(RingBuffer& buffer, T& data);
int SendSetClntIDPacket(int sock, int clnt_id);
int SendCreateGameRoomPacket(int sock, int roomID);
int SendJoinGameRoomPacket(int sock, int roomID);

int main(void)
{
    struct sockaddr_in serv_addr;
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERV_ADDR);
	serv_addr.sin_port=htons(SERV_PORT);

    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    SendSetClntIDPacket(clnt_sock, 123);

    exit(1);
}

int SendClosePacket(int sock) {
    BasePacketHeader header = {TCP_PACKET_START_CODE, 0, HANDLER_USER, HANDLER_USER_CLOSE, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    char send_buf[BUFFER_MAX_SIZE] = {};

    int packet_len = -1;

    packet_len = GetnerateBasePacket(send_buf, &header, NULL, &trailer);
    if (write(sock, send_buf, packet_len) < packet_len) {
        perror("write");
        return -1;
    }
    //close(clnt_sock);

    return 0;
}

int SendSetClntIDPacket(int sock, int clnt_id) {
    SetClntIDData sendData;
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(sendData), HANDLER_USER, HANDLER_USER_SETCLNTID, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};
    char buf[BUFFER_MAX_SIZE] = {};

    size_t packet_len = -1;
    size_t read_len = -1;

    RingBuffer recvBuffer;
    ReturnRoomData retData;
    int ret;

    sendData.clnt_id = clnt_id;
    packet_len = GetnerateBasePacket(buf, &header, &sendData, &trailer);
    if (write(sock, buf, packet_len) < packet_len) {
        perror("write");
        return -1;
    }

    while ((ret = UnpackData(recvBuffer, retData)) == 0) {
        read_len = read(sock, buf, BUFFER_MAX_SIZE);
        recvBuffer.enqueue(buf, read_len);
    }
    if (ret == -1) {
        return -1;
    }
    if ((!retData.isSuccess)) {
        return -1;
    }
    
    return 0;
}

int SendCreateGameRoomPacket(int sock, int roomID) {
    CreateGameRoomData sendData;
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(sendData), HANDLER_GAMEROOM, HANDLER_GAMEROOM_CREATE, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};
    char buf[BUFFER_MAX_SIZE] = {};

    size_t packet_len = -1;
    size_t read_len = -1;

    RingBuffer recvBuffer;
    ReturnRoomData retData;
    int ret;

    sendData.roomID = roomID;
    packet_len = GetnerateBasePacket(buf, &header, &sendData, &trailer);
    if (write(sock, buf, packet_len) < packet_len) {
        perror("write");
        return -1;
    }

    while ((ret = UnpackData(recvBuffer, retData)) == 0) {
        read_len = read(sock, buf, BUFFER_MAX_SIZE);
        recvBuffer.enqueue(buf, read_len);
    }
    if (ret == -1) {
        return -1;
    }
    if ((!retData.isSuccess) || retData.roomInfo.roomID != roomID) {
        return -1;
    }
    
    return 0;
}

int SendJoinGameRoomPacket(int sock, int roomID) {
    JoinGameRoomData sendData;
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(sendData), HANDLER_GAMEROOM, HANDLER_GAMEROOM_JOIN, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};
    char buf[BUFFER_MAX_SIZE] = {};

    size_t packet_len = -1;
    size_t read_len = -1;

    RingBuffer recvBuffer;
    ReturnRoomData retData;
    int ret;

    sendData.roomID = roomID;
    packet_len = GetnerateBasePacket(buf, &header, &sendData, &trailer);
    if (write(sock, buf, packet_len) < packet_len) {
        perror("write");
        return -1;
    }

    while ((ret = UnpackData(recvBuffer, retData)) == 0) {
        read_len = read(sock, buf, BUFFER_MAX_SIZE);
        recvBuffer.enqueue(buf, read_len);
    }
    if (ret == -1) {
        return -1;
    }
    if ((!retData.isSuccess) || retData.roomInfo.roomID != roomID) {
        return -1;
    }
    
    return 0;
}

int HeaderCheck(BasePacketHeader& header) {
    if (header.startCode != TCP_PACKET_START_CODE) {
        return -1;
    }

    return sizeof(BASE_PACKET_HEADER) + header.payloadLen + sizeof(BASE_PACKET_TRAILER);
}

int TrailerCheck(BasePacketTrailer& trailer) {
    if (trailer.endCode != TCP_PACKET_END_CODE) {
        return -1;
    }

    return 0;
}

//ret success 1, suspend 0, error -1
template <typename T>
int UnpackData(RingBuffer& buffer, T& data) {
    BasePacketHeader header;
    BasePacketTrailer trailer;

    size_t useSz = buffer.getUseSize();
    int packetSz = -1;
    
    if (useSz < sizeof(header) + sizeof(trailer))
        return 0;

    buffer.peek(&header, sizeof(BASE_PACKET_HEADER));
    if ((packetSz = HeaderCheck(header)) < 0) {
        buffer.flush();
        return -1;
    }
    if (useSz < packetSz) {
        return 0;
    }
    buffer >> header;
    buffer >> data;
    buffer >> trailer;
    if (TrailerCheck(trailer) < 0) {
        return -1;
    }

    return 1;
}