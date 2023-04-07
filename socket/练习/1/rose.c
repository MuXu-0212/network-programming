#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <arpa/inet.h>


int main(int argc, char const *argv[])
{
	//1.创建套接字
	int rose = socket(AF_INET, SOCK_STREAM, 0);
	if (rose == -1)
	{
		perror("craete socket fialed\n");
		return -1;
	}

	//2.进行连接
	struct sockaddr_in jack_info;
	jack_info.sin_family 		= AF_INET;
	jack_info.sin_port       	= htons(40000);
	jack_info.sin_addr.s_addr 	= inet_addr(argv[1]);  //把本地字节序IP地址转换为网络字节序IP地址

	int ret = connect(rose, (struct sockaddr *)&jack_info, sizeof(jack_info));
	if (ret == -1)
	{
		perror("connect sever fialed\n");
		return -1;
	}

	//3.数据发送
	char send_data[100];
	while(1)
	{
		memset(send_data, 0 ,sizeof(send_data));
		printf("请输入要发送的数据\n");
		scanf("%s",send_data);

		send(rose, send_data, strlen(send_data), 0);
		if (strcmp(send_data, "quit") == 0)
		{
			break;
		}
	}

	//4.下线并关闭掉套接字
	close(rose);
	return 0;
}