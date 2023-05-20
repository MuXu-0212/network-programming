////////////////////////////////////////////////////////
//
//  Copyright(C), 2005-2022, GEC Tech. Co., Ltd.
//
//  文件: socketOpt-UDPbroadcast.c
//  描述: 获取和设定UDP的广播属性
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
    // UDP套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 获取套接字的广播属性
    int val;
    int len = sizeof(val);
    getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &val, &len);
    printf("广播属性: %s\n", val==0?"OFF":"ON");

    // 设定套接字的广播属性为1
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    // 再次获取套接字的广播属性
    getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &val, &len);
    printf("广播属性: %s\n", val==0?"OFF":"ON");

    return 0;
}