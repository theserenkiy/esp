#include <stdio.h>
#include <string.h>

#include "lib.c"

#define BUFSIZE 1024

typedef struct {
	char to[16];
	char msg[1024];
} sendmsg_t;

int main() 
{
	int sockfd = connectToServer();

	sendmsg_t msg;

	printf("To: \n");
	fgets(msg.to, sizeof(msg.to), stdin);
	printf("Message: \n");
	fgets(msg.msg, sizeof(msg.msg), stdin);

	sendCommand(sockfd,2,&msg,sizeof(msg));
	
	char buf[BUFSIZE];
	receiveResponse(sockfd,buf);

	printf("%s\n",buf);

	//закрываем соединение
	close(sockfd);
	return 0;
}
