#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define HOST_IP "127.0.0.1"
#define HOST_PORT 1337

#define TOKEN "W48H4TKMBJPX6B5"

typedef struct {
	char name[32];
	int age;
}nigga_t;

typedef struct {
	char token[16];
	int cmd;
	int datalen;
} request_header_t;

typedef struct {
	int status;
	int length;
} response_header_t;

int connectToServer()
{
	// идентификатор сокета
	int sockfd;			

	// структура для хранения данных об адресе сервера
	struct sockaddr_in serveraddr;	

	//инициализация адреса сервера (куда подключаемся)
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(HOST_IP);
	serveraddr.sin_port = htons(HOST_PORT);

	//Создаём сокет
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{ 
		printf("ERROR opening socket\n");
		return -1;
	}

	//Подключаемся к серверу
	if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		printf("ERROR connecting\n");
		return -1;
	}
	
	return sockfd;
}

int sendCommand(int sockfd, int cmd, void *payload, int payload_size)
{
	request_header_t request_header; 
	sprintf(request_header.token,"%s",TOKEN);
	request_header.cmd = cmd;
	request_header.datalen = payload_size;
	write(sockfd, &request_header, sizeof(request_header_t));

	if(payload)
		write(sockfd, payload, payload_size);
}

int receiveResponse(int sockfd, void *pbuf)
{
	response_header_t header;
	//Читаем из сокета (принимаем сообщение с сервера)
	int nbytes = read(sockfd, &header, sizeof(response_header_t));
	if (nbytes < 0) 
		printf("ERROR reading from socket\n");

	printf("Response header: status=%d, length=%d\n", header.status, header.length); 

	if(header.length)
	{
		nbytes = read(sockfd, pbuf, header.length);
		if (nbytes < 0) 
			printf("ERROR reading from socket\n");
	}
}
