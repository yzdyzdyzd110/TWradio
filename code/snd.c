#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "msg.h"
 
int main()
{
	key_t key;
	pid_t pid;
	int msgid;
	
		
	struct msgbuf msg; 
		
	//生成消息队列的key
	if((key = ftok("./",'s'))==-1){
		perror("ftok error.");
		exit(0);
	}

	msgid = msgget(key,MSG_Q_PERM|IPC_CREAT);
        printf("请输入登录用户名:");
	fgets(msg.user_name,MSG_SIZE,stdin);
	msg.user_name[strlen(msg.user_name)-1]='\0';
	char my_name[MSG_SIZE];
	strcpy(my_name,msg.user_name);
	msg.mtype = HOST;
	msg.subtype = 1;
	msg.pid = getpid();
	pid = getpid();
	msgsnd(msgid,&msg,MSG_LEN,0);	//MSG_SIZE是消息正文大小
	
        msgrcv(msgid,&msg,MSG_LEN,GUEST,0);
	printf("Guest is online, two ways are established. Other side is %s.\nEnter exit to quit\n",msg.user_name);
        
	if((pid = fork())==-1)
	{
		perror("fork error.");
		exit(1);
	}
	else if(pid == 0)//子进程收
	{
		while (1)
		{
			msgrcv(msgid,&msg,MSG_LEN,GUEST,0);
			if(msg.subtype ==3)
			{	
				msg.mtype = HOST;
				msg.subtype = 4;
				msgsnd(msgid,&msg,MSG_LEN,0);
				kill(pid,SIGUSR1);
				exit(0);
			}
			switch (msg.subtype)
			{
			case 2:
				printf("[%s]%s\n",msg.user_name,msg.mtext);
				break;
			case 4:
				printf("[%s Logout]\n",msg.user_name);
				exit(0);
				break;
			default:
				break;
			}
		}
		

	} 
	else//父进程发
	{	
		strcpy(msg.user_name,my_name);
		msg.mtype = HOST;
		msg.subtype = 2;	//普通消息

		while(1)
		{	
			//printf("[%s]:",msg.user_name);
			fgets(msg.mtext,MSG_SIZE,stdin);			
			msgsnd(msgid,&msg,MSG_LEN,0);
			//当此进程输入exit时，表示进程退出聊天
			//本进程发出msg.type = 3消息，并wait进行阻塞等待子进程结束
			//对方进程收到后发出msg.subtype = 4消息，kill掉父进程，自己跳出
			//本进程收到后子进程跳出并结束循环
			if(strncmp(msg.mtext,"exit",4)==0)
			{
				msg.subtype = 3;
				msgsnd(msgid,&msg,MSG_LEN,0);
				break;
			}
		}
		wait(NULL);
	}
	
        
        return 0;
}