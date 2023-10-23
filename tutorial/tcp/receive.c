#include <stdio.h>
#include <string.h>

#include "lib.c"

#define BUFSIZE 1024


int main() 
{
	int sockfd = connectToServer();
	char buf[BUFSIZE];

	int nlines = 4;
	sendCommand(sockfd,3,&nlines,4);

	receiveResponse(sockfd,buf);

	printf("%s\n",buf);

	//закрываем соединение
	close(sockfd);
	return 0;
}
