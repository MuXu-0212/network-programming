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
	int jack = socket(AF_INET, SOCK_STREAM, 0);
	if (jack == -1)
	{
		perror("craete socket fialed\n");
		return -1;
	}

	//2.绑定自己的信息
	struct sockaddr_in jack_info;
	jack_info.sin_family = AF_INET;
	jack_info.sin_port       = htons(40000);
	//inet_addr("0.0.0.0"); 等价于 INADDR_ANY 
	jack_info.sin_addr.s_addr = inet_addr("0.0.0.0");  //把本地字节序IP地址转换为网络字节序IP地址

	int ret = bind(jack, (struct sockaddr *)&jack_info, sizeof(jack_info));   //绑定服务器自己的信息
	if(ret == -1)
	{
		perror("bind socket fialed\n");
		return -1;
	}

	//3.设置监听
	listen(jack, 10);


	//4.等待客户端连接    1.阻塞等待 2.利用返回值获取到连接端的套接字  3.可以将对方的信息进行储存
	struct sockaddr_in dest_info;  //用于储存连接上服务器的客户端信息
	int len = sizeof(dest_info);
	int new_sock = accept(jack, (struct sockaddr *)&dest_info, &len);
	if (new_sock == -1)
	{
		perror("accept failed\n");
		return -1;
	}

	//5.收发数据
	char recv_data[100];

	while(1)
	{
		memset(recv_data, 0 ,sizeof(recv_data));
		//int sockfd : 用于指定接收谁的数据
		//void *buf  : 用于存放数据的缓冲区
		//size_t len : 你要接收多少个字节
		//int flags  : 填充为 0
		recv(new_sock, recv_data, sizeof(recv_data), 0);
		printf("recv data is : %s\n", recv_data);
		if (strcmp(recv_data, "quit") == 0)
		{
			break;
		}
	}


	//6.关闭程序
	shutdown(new_sock, SHUT_RDWR);
	close(jack);

	return 0;
}




// 功能 			谁发送的 	发送给谁的 					发送什么样的数据
// 1 点对点聊天		3        指定一个你想发送的套接字      你在干嘛
// 2 点对点发送文件
// 3 群聊
// 4 打印好友列表



// 服务器   (把所有连接上服务器的套接字进行储存(1.数组 2,链表 3,结构体数组))

// 1. read();
// 2. 把数据进行拆分
// 3. send转发给指定的套接字(看对方是否在线,在线就发送，不在线则用服务器发送一个对方不在线，给客户端)
// 4. 如果要群发，也就是意味着 把储存套接字当中的所有套接字发送一遍.