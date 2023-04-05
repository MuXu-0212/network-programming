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

typedef struct list
{
	//数据域
	char admin[100];
	char password[100];
	char name[100];
	int connfd;
	int room;
	int online;
	//下标域
	struct list *next;
}list,*plist;

plist head;
int i=0;
FILE *fp;
int source_fd;
pthread_t tid[5];
pthread_t ttid[5];


plist init_list()
{
	plist list=malloc(sizeof(list));
	if(list==NULL)
	{
		printf("list error\n");
		return NULL;
	}
	list->next=NULL;
	return list;
}


plist init_new(char *adm,char *pas,char *nam)
{
	plist newnode=malloc(sizeof(list));
	if(newnode==NULL)
	{
		printf("newnode error\n");
		return NULL;
	}
	strcpy(newnode->admin,adm);
	strcpy(newnode->password,pas);
	strcpy(newnode->name,nam);
	newnode->room=0;
	newnode->connfd=0;
	newnode->online=0;
	newnode->next=NULL;
	return newnode;
}

int tail_insert(plist list,plist newnode)
{
	plist p=list;
	while(p->next!=NULL)
		p=p->next;
	p->next=newnode;
	newnode->next=NULL;
	return 0;
}

int display(plist list)
{
	plist p=list;
	printf("账号\t 密码\t 昵称\n");
	while(p->next!=NULL)
	{
		p=p->next;
		printf("%s\t%s\t%s\n",p->admin,p->password,p->name);
	}
	return 0;
}


void *gongneng(void *arg)
{
	int sockfd=*(int *)arg;
	plist p;
	char admin[100];
	char connfd[100];
	char name[100];
	char str[100];
	char buf[100];
	char num[100];
	while(1)
	{
		bzero(num,sizeof(num));
		printf("等待接收指令\n");
	    recv(sockfd,num,5,0);
		//退出
	    if(!strncmp(num,"zero",4))
		{
			p=head;
			while(p->next!=NULL)
			{
				p=p->next;
				if(p->connfd==sockfd)
				{
					printf("%d\n",p->connfd);
					printf("%d\n",sockfd);
					break;
				}
			}
			p->online=0;
			p->connfd=0;
			printf("%d is not connect\n",sockfd);
			i--;
			printf("目前可连接数目：%d\n",5-i);
			break;
		}
		//刷新查看在线列表
		if(!strncmp(num,"one",3))
		{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
			p=head;
			while(p->next!=NULL)
			{
				p=p->next;
				if(p->online==1)
				{
					bzero(admin,sizeof(admin));
					bzero(name,sizeof(name));
					strcpy(name,p->name);
					send(sockfd,name,strlen(name),0);
					usleep(1000);
					strcpy(admin,p->admin);
					send(sockfd,admin,strlen(admin),0);
					usleep(1000);
				}
			}
			if(p->next==NULL)
			{
				printf("刷新完毕\n");
				send(sockfd,"OK",2,0);
			}
		}
		//进入聊天室
		if(!strncmp(num,"two",3))
		{
			p=head;
			while(p->next!=NULL)
			{
				p=p->next;
				if(p->connfd==sockfd)
					break;
			}
			p->room=1;
			bzero(str,sizeof(str));
			bzero(name,sizeof(name));
			sprintf(str,"%s加入聊天室!",p->name);
			strcpy(name,p->name);
			p=head;
			while(p->next!=NULL)
			{
				p=p->next;
				if(p->room==1&&p->connfd!=sockfd)
					send(p->connfd,str,strlen(str),0);
			}
			while(1)
			{
				bzero(str,sizeof(str));
				recv(sockfd,str,sizeof(str),0);
				if(!strncmp(str,"quit",4))
				{
					p=head;
					while(p->next!=NULL)
			        {
				        p=p->next;
				        if(p->connfd==sockfd)
					        break;
			        }
					p->room=0;
					p=head;
					sprintf(str,"%s退出聊天室!\n",name);
					printf("str:%s",str);
					while(p->next!=NULL)
				    {
						p=p->next;
					    if(p->room==1&&p->connfd!=sockfd)
					         send(p->connfd,str,strlen(str),0);
				    }
					break;
				}
				p=head;
				bzero(buf,sizeof(buf));
				sprintf(buf,"%s:%s",name,str);
				while(p->next!=NULL)
				{
					p=p->next;
					if(p->room==1&&p->connfd!=sockfd)
					    send(p->connfd,buf,strlen(buf),0);
				}
			}
		}
		//上传文件
		if(!strncmp(num,"three",5))
		{
			bzero(str,sizeof(str));
			bzero(buf,sizeof(buf));
			recv(sockfd,str,sizeof(str),0);
			if(!strncmp(str,"OK",2))
			{
				recv(sockfd,buf,sizeof(buf),0);
				bzero(str,sizeof(str));
				sprintf(str,"./fail/%s",buf);
				source_fd = open(str,O_RDWR|O_CREAT|O_TRUNC);//打开源路径
				if(source_fd == -1 )
					printf("创建文件 %s\n 失败！\n",str);
				else
				{
					send(sockfd,"OK",2,0);
					while(1)
					{
						bzero(buf,sizeof(buf));
						recv(sockfd,buf,sizeof(buf),0);
						if(!strncmp(buf,"quit",4))
						{
							printf("接收成功！\n");
							break;
						}
						write(source_fd,buf,strlen(buf));
					}
					close(source_fd);
				}
			}
		}
		//下载文件
		if(!strncmp(num,"four",4))
		{
			DIR *dir=opendir("./fail");
			struct dirent *mydir;
			while((mydir=readdir(dir))!=NULL)
			{
				if(strcmp(mydir->d_name,".")!=0&&strcmp(mydir->d_name,"..")!=0)
				{
					
					if(mydir->d_type==DT_REG)
			        {
						bzero(str,sizeof(str));
						printf("%s\n",mydir->d_name);
						strcpy(str,mydir->d_name);
						send(sockfd,str,strlen(str),0);
						recv(sockfd,str,sizeof(str),0);
						{	
						}
					}
				}	
			}
			send(sockfd,"quit",4,0);
			
			
			bzero(str,sizeof(str));
			bzero(buf,sizeof(buf));
			recv(sockfd,str,sizeof(str),0);
			printf("str = %s\n",str);
			sprintf(buf,"./fail/%s",str);
			source_fd = open("./fail/1.txt",O_RDONLY);
			if(source_fd <= 0)
				printf("1111\n");
			printf("fd = %d\n",source_fd);
			int nread;
			bzero(buf,sizeof(buf));
			recv(sockfd,str,sizeof(str),0);
			{
				
			}
			while((nread = read(source_fd,buf,100)) > 0)  //读取源文件的内容
			{	
				printf("%s\n",buf);
			    //bzero(buf,sizeof(buf));
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
	}
}

void *user_login(void *arg)//用户登录注册
{
	int sockfd=*(int *)arg;
	//pthread_detach(pthread_self());
	plist p;
	char admin[100];
	char password[100];
	char name[100];
	char str[100];
	char num[100];
	
	//登录
	while(1)
	{
		bzero(num,sizeof(num));
	    recv(sockfd,num,4,0);
	    if(!strncmp(num,"one",3))
	    {
		    printf("正在等待接收登录信息\n");
		    while(1)
		    {
				p=head->next;
				bzero(admin,sizeof(admin));
				bzero(password,sizeof(password));
				bzero(name,sizeof(name));
				
				recv(sockfd,admin,sizeof(admin),0);
				usleep(1000);
				printf("admin:%s\n",admin);
				
				recv(sockfd,password,sizeof(password),0);
				usleep(1000);
				printf("password:%s\n",password);
		
				while(p!=NULL)
				{
					if(!strcmp(p->admin,admin)&&!strcmp(p->password,password))
						break;
					p=p->next;
				}
				if(p==NULL)
					send(sockfd,"账号或密码错误",21,0);
				if(p->online==1)
				{
					send(sockfd,"该用户已在线",18,0);
				}
				else 
				{
					send(sockfd,"OK",2,0);
					p->connfd=sockfd;
					p->online=1;
					strcpy(name,p->name);
					sleep(1);
					send(sockfd,name,strlen(name),0);
					pthread_create(&ttid[i],NULL,gongneng,&sockfd);
					pthread_join(ttid[i],NULL);
					break;
				}
				
		   }
	    }
	//注册
		if(!strncmp(num,"two",3))
		{
			printf("正在等待接收注册信息\n");
			while(1)
			{
				p=head;
				bzero(admin,sizeof(admin));
				bzero(password,sizeof(password));
				bzero(name,sizeof(name));
				bzero(str,sizeof(str));
				recv(sockfd,admin,sizeof(admin),0);
				sleep(1);
				printf("admin:%s\n",admin);
				
				recv(sockfd,password,sizeof(password),0);
				sleep(1);
				printf("password:%s\n",password);
		
				recv(sockfd,name,sizeof(name),0);
				sleep(1);
				printf("name:%s\n",name);
				while(p->next!=NULL)
				{
					p=p->next;
					if(!strcmp(p->admin,admin)||!strcmp(p->password,password)||!strcmp(p->name,name))
					{
						send(sockfd,"用户已存在",15,0);
						break;
					}
				}
				if(p->next==NULL)
				{
					fp=fopen("./menu/menu.txt","a+");
					sprintf(str,"admin:%s password:%s name:%s\n",admin,password,name);
					fwrite(str,strlen(str),1,fp);
					plist new=init_new(admin,password,name);
		            tail_insert(head,new);
					display(head);
					send(sockfd,"OK",2,0);
					fclose(fp);
					break;
				}
			}
		}
		//退出
		if(!strncmp(num,"zero",4))
		{
			printf("%d is not connect\n",sockfd);
			i--;
			printf("目前可连接数目：%d\n",5-i);
			break;
		}
	}
}

int main(int argc,char *argv[])
{
	//创建一个TCP协议的套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("make socket error\n");
		return -1;
	}
	//定义一个存放自己的IP地址的结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	//将IP地址和端口号存放进结构体中
	server_addr.sin_family = AF_INET;//地址族
	server_addr.sin_port   = htons(atoi(argv[2])); //端口号
	inet_pton(AF_INET,argv[1],&server_addr.sin_addr);//IP地址
	//服务器绑定自己的IP地址
	bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	//将绑定好的套接字设置为监听套接字
	listen(sockfd,5);
	//定义一个存放连接对象的IP地址和端口号等信息的结构体
	struct sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	bzero(&client_addr,client_addr_size);

	head = init_list();
	
	char adm[100];
	char pas[100];
	char nam[100];
	char temp[100];
	bzero(adm,sizeof(adm));
	bzero(pas,sizeof(pas));
	bzero(nam,sizeof(nam));
	bzero(temp,sizeof(temp));
	fp=fopen("./menu/menu.txt","a+");
	while(fgets(temp,100,fp))
	{
		bzero(adm,sizeof(adm));
	    bzero(pas,sizeof(pas));
	    bzero(nam,sizeof(nam));
		sscanf(temp,"admin: %s password: %s name: %s",adm,pas,nam);
		plist new=init_new(adm,pas,nam);
		tail_insert(head,new);
	}
	fclose(fp);
	display(head);
	int newfd;
	
	//实时接收客户端连接
	while(1)
	{   
		//等待客户端的连接
	    newfd = accept(sockfd,(struct sockaddr *)&client_addr,&client_addr_size);
		if (newfd>0&&i<5)
		{
			printf("%s is connect\n",inet_ntoa(client_addr.sin_addr));
			i++;
			printf("目前可连接数目：%d\n",5-i);
			pthread_create(&tid[i],NULL,user_login,&newfd);
		}
		else
			printf("连接人数已满！\n");
	}
	return 0;
}



