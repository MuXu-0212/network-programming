////////////////////////////////////////////////////////
//
//  Copyright(C), 2005-2022, GEC Tech. Co., Ltd.
//
//  文件: socketOpt-TCPbuffer.c
//  描述: 获取和设定TCP的数据缓冲区的大小
//
///////////////////////////////////////////////////////

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int sndbuf, rcvbuf;
    int len1 = sizeof(sndbuf);
    int len2 = sizeof(rcvbuf);
    getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &len1);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len2);
    printf("发送缓冲区大小: %d\n", sndbuf);
    printf("接收缓冲区大小: %d\n", rcvbuf);

    // 设定套接字的缓冲区
    sndbuf = 1024;
    rcvbuf = 2048;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(len1));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(len2));

    // 再次获取套接字的缓冲区属性
    getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &len1);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len2);
    printf("发送缓冲区大小: %d\n", sndbuf);
    printf("接收缓冲区大小: %d\n", rcvbuf);

    close(sockfd);
    return 0;
}