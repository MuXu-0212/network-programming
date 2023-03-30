#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
	//1. 创建未连接套接字对象   socket
	//参数 int domain : 协议的选择 (你要协议通信) AF_INET 使用APV4的协议进行通信
	//参数 int type   : TCP UDP通信类型  SOCK_STREAM TCP协议   SOCK_DGRAM UDP协议
	//参数 int protocol : 补充协议,如果没有直接填写为0
	//返回值：成功返回一个新的文件描述符作为套接字 失败 -1
	int jack = socket(AF_INET, SOCK_STREAM, 0);
	if (jack == -1)
	{
		printf("创建套接字失败\n");
		return -1;
	}

	// 2. 绑定端口号和IP地址 bind
	// int sockfd：套接字 你要绑定哪一个套接字
	// const struct sockaddr *addr : 包含了本机的 IP 端口号的结构体 (需要用户自己设置)
	// socklen_t addrlen : 你传入的结构体大小
	//返回值 成功 返回0 失败返回-1;
	struct sockaddr_in jack_addr;   					//创建 服务其
	memset(&jack_addr, 0 , sizeof(jack_addr));

	jack_addr.sin_family = AF_INET;							//使用IPV4的方式进行通信
	jack_addr.sin_port   = htons(atoi(argv[1]));    		// 端口 ：我们在终端输入是 字符串类型,我们的端口号是短整型,所以要进行atoi转换
															// 网络是一个大端序发送数据,所以需要把本地字节序变为网络字节序
	//jack_addr.sin_addr.s_addr = htonl("192.168.12.177");  //把本地字节序转换为网络字节序
	jack_addr.sin_addr.s_addr = htonl(INADDR_ANY);          //INADDR_ANY 代表自动获取本机IPV4地址
	int ret = bind(jack, (struct sockaddr *)&jack_addr,sizeof(jack_addr));  //绑定服务器的IP地址与端口号
	if(ret != 0)
	{
		printf("绑定失败\n");
		return -1;
	}

	// 3. 给等待连接的套接字设置监听                    listen
	// int sockfd  : 待链接的套接字
	// int backlog : 同时能够接受多少个链接请求.
	listen(jack, 1);


	// 4. 无限等待客户端的连接，获取客户端通信套接字     accept
	// int sockfd : 服务器自己的套接字
	// struct sockaddr *addr : 获取到对方的信息保存到该变量当中
	// socklen_t *addrlen : 结构体的大小
	// 返回值 成功则为 接收端的套接字 失败 -1
	struct sockaddr_in rose;			 //把连接端的信息进行保存;
	socklen_t addr_size = sizeof(rose);  //获取到结构体的大小
	int rose_fd = accept(jack, (struct sockaddr *)&rose, &addr_size);
	if (rose_fd == -1)
	{
		printf("链接失败\n");
	}

	printf("当前链接的对象为 : %d\n", rose_fd);
	// 5. 客户端和服务端通信                           read write   send recv
	char recv_data[100];
	while(1)
	{
		memset(recv_data, 0, sizeof(recv_data));

		read(rose_fd, recv_data, sizeof(recv_data));
		printf("我收到了%d 发送过来的 :%s \n", rose_fd, recv_data);

		if (strcmp(recv_data, "byebye") == 0)
		{
			printf("对方以下线\n");
			break;
		}
	}

	// 6. 关闭服务端                                  close
	close(jack);
	return 0;
}
