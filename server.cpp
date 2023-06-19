#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>

#include "socket.h"
#include "epoll.h"
#include "Logger.h"

#define SERV_ADDR INADDR_ANY
#define SERV_PORT 1234
#define EPOLL_SIZE 5000

using namespace std;

int main(void)
{
    int serv_sock = -1;
    int epfd = -1, event_cnt = 0;

    struct epoll_event *ep_events;

    Logger::LoggerSetting(LOGLEVEL::DEBUG);
    Logger logger("../logfile", "log.txt");

    logger.Log(LOGLEVEL::INFO, "Server Start...");

    if ((serv_sock = SetTCPServSock(SERV_ADDR, SERV_PORT, SOMAXCONN, true)) < 0) {
        logger.Log(LOGLEVEL::ERROR, "SetTcpServSock: %s", strerror(errno));
        exit(1);
    }

    if ((epfd = InitEpoll(&ep_events, EPOLL_SIZE)) < 0) {
        logger.Log(LOGLEVEL::ERROR, "InitEpoll: %s", strerror(errno));
        exit(1);
    }

    if (SetETServSock(epfd, serv_sock) < 0) {
        logger.Log(LOGLEVEL::ERROR, "SetETServSock: %s", strerror(errno));
        exit(1);
    }

    do
    {
        if ((event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1)) < 0) {
            logger.Log(LOGLEVEL::ERROR, "epoll_wait: %s", strerror(errno));
            //Todo: exit이 맞을지
        }

        for (int i=0; i < event_cnt; i++) {

        }
    } while (true);

    logger.Log(LOGLEVEL::INFO, "Server End...");

    close(epfd);
    close(serv_sock);
    exit(0);
}