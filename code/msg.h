#define MSG_TO_SERVER 10
#define MSG_SIZE 255
#define MSG_Q_PERM 0666
#define HOST 1
#define GUEST 2


struct msgbuf{
    long mtype;
    int subtype;    //消息请求类型
    int pid;	//发送消息进程号
    char user_name[MSG_SIZE];
    char mtext[MSG_SIZE];
}msg;

#define MSG_LEN (sizeof(struct msgbuf) - sizeof(long)) 
