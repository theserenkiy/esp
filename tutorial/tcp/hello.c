#include <stdio.h>
#include <string.h>

#include "lib.c"

#define BUFSIZE 1024


int main() 
{
	int sockfd = connectToServer();
	char buf[BUFSIZE];

	sendCommand(sockfd,1,NULL,0);
	
	receiveResponse(sockfd,buf);

	printf("%s\n",buf);

	//закрываем соединение
	close(sockfd);
	return 0;
}
