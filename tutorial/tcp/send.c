#include <stdio.h>
#include <string.h>

#include "lib.c"

#define BUFSIZE 1024


int main() 
{
	int sockfd = connectToServer();
	char buf[BUFSIZE];

	printf("Enter message: \n");
	fgets(buf, BUFSIZE, stdin);


	sendCommand(sockfd,2,buf,strlen(buf));
	
	receiveResponse(sockfd,buf);

	printf("%s\n",buf);

	//закрываем соединение
	close(sockfd);
	return 0;
}
