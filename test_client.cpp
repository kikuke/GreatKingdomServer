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

int main(void)
{
    struct sockaddr_in serv_addr;
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    int readLen = -1;
    int writeLen = -1;

    char send_buf[BUFFER_MAX_SIZE] = {};
    char recv_buf[BUFFER_MAX_SIZE] = {};
    BasePacketHeader header = {TCP_PACKET_START_CODE, sizeof(EchoData), HANDLER_ECHO, HANDLER_ECHO_ECHOTEST, 0, 0};
    BasePacketTrailer trailer = {TCP_PACKET_END_CODE};

    EchoData echoData;
    size_t packetLen = -1;

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERV_ADDR);
	serv_addr.sin_port=htons(SERV_PORT);

    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while (true) {
        readLen = read(STDIN_FILENO, echoData.msg, ECHO_MAX_SIZE);
        packetLen = GetnerateBasePacket(send_buf, &header, &echoData, &trailer);
        writeLen = write(clnt_sock, send_buf, packetLen);
        printf("write: %d\n", writeLen);
        read(clnt_sock, recv_buf, BUFFER_MAX_SIZE);
        printf("%s", recv_buf);
    }

    close(clnt_sock);

    exit(1);
}