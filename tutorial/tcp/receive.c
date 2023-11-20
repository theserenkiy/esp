#include <stdio.h>
#include <string.h>

#include "lib.c"

#define BUFSIZE 1024

typedef struct {
	int limit;
	int all;
} read_header_t;

int main() 
{
	while(1)
	{
		int sockfd = connectToServer();
		char buf[BUFSIZE];

		read_header_t header = {
			.limit = 100,
			.all = 1
		};

		int nlines = 4;
		sendCommand(sockfd,3,&nlines,8);

		int nbytes = receiveResponse(sockfd,buf);

		if(nbytes)
			printf("%s\n",buf);

		//закрываем соединение
		close(sockfd);
		sleep(1);
	}
	return 0;
}
