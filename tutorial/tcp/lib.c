#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define HOST_IP "37.46.135.97"
#define HOST_PORT 1338

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
	// структура для хранения данных об адресе сервера
	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET,
		.sin_port = htons(HOST_PORT),
		.sin_addr.s_addr = inet_addr(HOST_IP)
	};	

	//Создаём сокет
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
	{ 
		printf("ERROR opening socket\n");
		return -1;
	}

	//Подключаемся к серверу
	if (connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		printf("ERROR connecting\n");
		return -1;
	}
	
	return sd;
}

int sendCommand(int sd, int cmd, void *payload, int payload_size)
{
	request_header_t header; 
	sprintf(header.token,"%s",TOKEN);
	header.cmd = cmd;
	header.datalen = payload_size;
	write(sd, &header, sizeof(request_header_t));

	if(payload)
		write(sd, payload, payload_size);
}

int receiveResponse(int sd, void *pbuf)
{
	response_header_t header;
	//Читаем из сокета (принимаем сообщение с сервера)
	int nbytes = read(sd, &header, sizeof(response_header_t));
	if (nbytes < 0) 
		printf("ERROR reading from socket\n");

	printf("Response header: status=%d, length=%d\n", header.status, header.length); 

	if(header.length)
	{
		nbytes = read(sd, pbuf, header.length);
		if (nbytes < 0) 
			printf("ERROR reading from socket\n");
	}
}
