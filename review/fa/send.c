#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
int source_fd;

void look(int sockfd)
{
	char admin[100];
	char name[100];
	char str[100];
	printf("\n昵称\t账号\n");
	while(1)
	{
		bzero(admin,sizeof(admin));
		bzero(name,sizeof(name));
		recv(sockfd,name,sizeof(name),0);
		if(!strncmp(name,"OK",2))
			break;	
		usleep(1000);
		recv(sockfd,admin,sizeof(admin),0);
		printf("%s\t%s\n",name,admin);
		
	}
	printf("刷新完毕\n\n");
}

void *revr(void *arg)
{
	int sockfd=*(int *)arg;
	char str[100];
	while(1)
	{
		bzero(str,sizeof(str));
        recv(sockfd,str,sizeof(str),0);
		printf("%s\n",str);
	}
}

void room(int sockfd)
{
	printf("已加入聊天室!(quit退出)\n");
	pthread_t ttid;
	pthread_create(&ttid,NULL,revr,&sockfd);
	char str[100];
	while(1)
	{
		bzero(str,sizeof(str));
		scanf("%s",str);
		send(sockfd,str,strlen(str),0);
		if(!strncmp(str,"quit",4))
		{
			printf("已退出聊天室!\n");
			break;
		}
	}
}

void take(int sockfd)
{
	char str[100];
	char buf[100];
	bzero(str,sizeof(str));
	printf("请输入要上传的文件名\n");
	scanf("%s",str);
	source_fd = open(str,O_RDONLY);//打开源路径，只读模式
	if(source_fd == -1 )
		printf("打开文件%s失败!\n",str);
	else
	{
		send(sockfd,"OK",2,0);
		printf("****\n");
		int ret=access(str,F_OK);
		struct stat copestat;//这个结构体来自#include <sys/stat.h>
		stat(str,&copestat);
		if(ret==0)
		{
			if(S_ISREG(copestat.st_mode))
			{
				send(sockfd,str,strlen(str),0);
				bzero(buf,sizeof(buf));
				recv(sockfd,buf,sizeof(buf),0);
				if(!strncmp(buf,"OK",2))
				{
					int nread;
					bzero(buf,sizeof(buf));
					while((nread = read(source_fd,buf,100)) > 0)  //读取源文件的内容
					{	
						printf("nread=%d\n",nread);
						send(sockfd,buf,nread,0);
						bzero(buf,sizeof(buf));
					}
					if(nread<=0)
					{
						send(sockfd,"quit",4,0);
						printf("上传完成\n\n");
						close(source_fd);
					}
				}
				else
					printf("上传失败\n\n");
			}
			else
				printf("权限不足\n\n");
		}
		else
			printf("格式输入错误\n\n");
	}
	
}

void get(int sockfd)
{
	char str[100];
	char buf[100];
	
	while(1)
	{
		bzero(str,sizeof(str));
		bzero(buf,sizeof(buf));
		recv(sockfd,str,sizeof(str),0);
		if(!strncmp(str,"quit",4))
			break;
		printf("%s\n",str);
		send(sockfd,"OK",2,0);
	}
	
	bzero(str,sizeof(str));
	bzero(buf,sizeof(buf));
	printf("请输入要下载的文件:");
	scanf("%s",str);
	send(sockfd,str,strlen(str),0);
	printf("请输入要保存的位置:");
	scanf("%s",buf);
	source_fd = open(str,O_RDWR|O_CREAT|O_TRUNC);
	send(sockfd,"OK",2,0);
	while(1)
	{
		bzero(buf,sizeof(buf));
		recv(sockfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
		if(!strncmp(buf,"quit",4))
		{
			printf("下载成功！\n");
			break;
		}
		
		write(source_fd,buf,strlen(buf));
	}
	close(source_fd);
}


void make(int sockfd)
{
	
}

void *caozuo(void *arg)
{
	int sockfd=*(int *)arg;
	
	while(1)
	{
		int num;
		printf("1:刷新查看在线列表\n");
		printf("2:进入聊天室\n");
		printf("3:上传文件\n");
		printf("4:下载文件\n");
		printf("0:退出\n");
		printf("请选择：");
		scanf("%d",&num);
		switch(num)
		{
			case 0:send(sockfd,"zero",4,0);exit(0);break;
			case 1:send(sockfd,"one",3,0);look(sockfd);break;
			case 2:send(sockfd,"two",3,0);room(sockfd);break;
			case 3:send(sockfd,"three",5,0);take(sockfd);break;
			case 4:send(sockfd,"four",4,0);get(sockfd);break;
		}
	}
}

void denglu(int sockfd)
{
	char admin[100];
	char password[100];
	char name[100];
	char str[100];
	pthread_t tid;
	while(1)
	{
		bzero(admin,sizeof(admin));
		bzero(password,sizeof(password));
		bzero(str,sizeof(str));
		printf("请输入账号:");
		scanf("%s",admin);
		send(sockfd,admin,strlen(admin),0);
		
		printf("请输入密码:");
		scanf("%s",password);
		send(sockfd,password,strlen(password),0);
		
		recv(sockfd,str,sizeof(str),0);
		printf("%s\n\n",str);
		if(!strncmp(str,"OK",2))
		{
			recv(sockfd,name,sizeof(name),0);
			printf("登录成功！欢迎用户:%s\n\n",name);
			break;
		}	
	}
	pthread_create(&tid,NULL,caozuo,&sockfd);
	pthread_join(tid,NULL);
	
}

void zhuce(int sockfd)
{
	
	char admin[100];
	char password[100];
	char name[100];
	char str[100];
	while(1)
	{
		bzero(admin,sizeof(admin));
		bzero(password,sizeof(password));
		bzero(name,sizeof(name));
		bzero(str,sizeof(str));
		printf("请输入账号:");
		scanf("%s",admin);
		send(sockfd,admin,strlen(admin),0);
		
		printf("请输入密码:");
		scanf("%s",password);
		send(sockfd,password,strlen(password),0);
			
		printf("请输入昵称:");
		scanf("%s",name);
		send(sockfd,name,strlen(name),0);
		
		recv(sockfd,str,sizeof(str),0);
		printf("%s\n\n",str);
		if(!strncmp(str,"OK",2))
		{
			printf("注册成功\n\n");
			break;
		}
	}
}


int main(int argc,char *argv[])
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        perror("make socket failed\b");
        return -1;
    }
    struct sockaddr_in server;
    bzero(&server,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&server.sin_addr);
    connect(sockfd,(struct sockaddr *)&server,sizeof(server));
   
    while(1)
	{
		int num;
		printf("欢迎进入聊天系统！\n");
		printf("1：登录\n");
		printf("2：注册\n");
		printf("0：退出\n");
		printf("请选择：");
		scanf("%d",&num);
		switch(num)
		{
			case 1:send(sockfd,"one",3,0);denglu(sockfd);break;
			case 2:send(sockfd,"two",3,0);zhuce(sockfd);break;
			case 0:send(sockfd,"zero",4,0);exit(0);break;
			default :printf("请输入正确的选择！\n");break;
		}
	}
    return 0;
}

