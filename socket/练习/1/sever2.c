#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <pthread.h>



struct userinfo
{
	int  clientsock; 				//客户端的套接字
	struct sockaddr_in  destaddr;  	//客户端的信息结构体
};

int count = 0;   //目前连接服务器的人数为0

void *Clientrecv(void *arg)
{
	struct userinfo *p = (struct userinfo *)arg;

	char recv_data[1024];
	while(1)
	{
		memset(recv_data, 0 , sizeof(recv_data));
		recv(p->clientsock, recv_data, sizeof(recv_data),0);
		printf("接收到来自 %s 端口号为 %d 的 %s\n", 
				inet_ntoa(p->destaddr.sin_addr),       //把网络字节序转换为本地字节序(因为网络大端序发送的数据,要把他转为本地字节序)
				ntohs(p->destaddr.sin_port),	   //把网络端口号转换为本地端口号
				recv_data);

		if(strcmp(recv_data, "byebye") == 0)
		{
			if (count > 0)
			{
				count--;
			}
			
			pthread_exit(NULL);
		}
	}
}


int main(int argc, char const *argv[])
{

	//1.创建套接字
	int sever = socket(AF_INET, SOCK_STREAM, 0);
	if (sever == -1)
	{
		perror("craete socket fialed\n");
		return -1;
	}

	//2.绑定自己的信息
	struct sockaddr_in jack_info;
	jack_info.sin_family 	= AF_INET;
	jack_info.sin_port      = htons(40002);
	//inet_addr("0.0.0.0"); 等价于 INADDR_ANY 
	jack_info.sin_addr.s_addr = inet_addr("0.0.0.0");  //把本地字节序IP地址转换为网络字节序IP地址

	int ret = bind(sever, (struct sockaddr *)&jack_info, sizeof(jack_info));   //绑定服务器自己的信息
	if(ret == -1)
	{
		perror("bind socket fialed\n");
		return -1;
	}

	//3.设置监听
	listen(sever, 10);


	//创建一个指针数组去存放所有连接上服务器客户端的信息
	struct userinfo *newclinet[10];

	
	pthread_t tid[10];
	int new_client;

	struct sockaddr_in destaddr;
	int len = sizeof(jack_info);
	while(1)
	{
		if (count < 10 )
		{
			printf("正在监听客户端连接...\n");
			new_client = accept(sever, (struct sockaddr *)&destaddr, &len);
			if (new_client == -1)
			{
				perror("accept failed\n");
				continue;
			}

			//因为结构体指针数组需要指向一个有信息的空间
			newclinet[count] = malloc(sizeof(struct userinfo));
			if (newclinet[count] == NULL)
			{
				printf("申请对空间失败\n");
				continue;
			}

			//需要把链接上服务器的信息储存起来
			newclinet[count]->clientsock = new_client;  //用于储存新客户端的套接字
			newclinet[count]->destaddr   = destaddr;    //保存连接服务器的客户端信息.

			//创建线程去接收客户端发送过来的数据
			pthread_create( &tid[count], NULL, Clientrecv ,(void *)newclinet[count]);

			//当我们以上操作都没有问题的时候, 便于记录下一个客户端的信息。
			count++;
		}
	}


	
	return 0;
}