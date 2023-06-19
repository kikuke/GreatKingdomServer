#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 1234

int main(void)
{
    struct sockaddr_in serv_addr;
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(SERV_ADDR);
	serv_addr.sin_port=htons(SERV_PORT);

    for (int i=0; i< 100; i++) {
        connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        close(clnt_sock);
    }

    exit(1);
}