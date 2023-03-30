#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <arpa/inet.h>


int main(int argc, char const *argv[])
{

// 1. 创建套接字    socket
	int rose = socket(AF_INET, SOCK_STREAM, 0);
	if (rose == -1)
	{
		printf("申请客户端套接字失败\n");
		return -1;
	}

	// 2. 连接服务器的端口号和IP地址      connect
	struct sockaddr_in jack_addr;
	jack_addr.sin_family = AF_INET;
	jack_addr.sin_port   = htons(atoi(argv[1]));    	//端口,服务器的端口号
	inet_pton(AF_INET, argv[2], &jack_addr.sin_addr);	//服务器的ip


	//链接服务器
	// int sockfd: 要连接服务器客户端的套接字
	//	const struct sockaddr *addr 你要链接哪一个服务器
	int ret = connect(rose, (struct sockaddr *)&jack_addr,sizeof(jack_addr));
	if (ret == -1)
	{
		printf("链接服务器失败\n");
		return -1;
	}

	// 3. 客户端和服务端通信              read write   send recv
	char send_data[100];
	while(1)
	{
		memset(send_data, 0 , sizeof(send_data));
		printf("请输入要发送的数据\n");
		scanf("%s",send_data);
		write(rose, send_data, strlen(send_data));
		if (strcmp(send_data, "byebye") == 0)
		{
			printf("退出聊天\n");
			break;
		}
	}

	// 4. 关闭客户端                     close
	close(rose);
	return 0;
}