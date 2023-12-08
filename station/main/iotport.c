#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include "wifi.c"
#include "freertos/freertos.h"

#define BUFSIZE 1024

#define HOST_IP "37.46.135.97"
#define HOST_PORT 3593

#define TOKEN "kIHIWuenAxIQeVEuxYDIaRaiQiXUVeJU"

typedef struct {
	char token[32];
	int cmd;
	int len;
	char payload[1024];
} req_header_t;

typedef struct {
	int error;
	int len;
	char payload[1024];
} res_header_t;

int connectToServer()
{
	//структура для хранения данных об адресе сервера
	struct sockaddr_in serveraddr;

	//инициализация адреса сервера (куда подключаемся)
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(HOST_IP);
	serveraddr.sin_port = htons(HOST_PORT);

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

int serverTransaction()
{
	int sd;
	int var_id = 1;
	sd = connectToServer();
	if(sd < 0)
	{
		return -1;
	}
	
	req_header_t req_header;

	memcpy(req_header.token, TOKEN, 32);
	req_header.cmd = 2;
	req_header.len = 4;
	
	//int value = 1;

	int* buf = (int *)req_header.payload;
	buf[0] = var_id;

	write(sd, &req_header, 32+4+4+4);

	res_header_t res_header;
	read(sd, &res_header, sizeof(res_header));

	int led_state = (unsigned int)*res_header.payload;

	printf("Error: %d, len: %d, payload: %d\n", res_header.error, res_header.len, led_state);

	if(!res_header.error)
	{
		gpio_set_level(12,!!led_state);
	}

	close(sd);
	return 0;
}

int iotport_getVar(int id, void *data)
{
	int sd;
	int var_id = 1;
	wait_for_wifi();
	sd = connectToServer();
	if(sd < 0)
	{
		return -1;
	}
	
	req_header_t req_header;

	memcpy(req_header.token, TOKEN, 32);
	req_header.cmd = 2;
	req_header.len = 4;
	
	int* buf = (int *) req_header.payload;
	buf[0] = var_id;

	write(sd, &req_header, 32+4+4+4);

	res_header_t res_header;
	read(sd, &res_header, sizeof(res_header));

	if(res_header.error)
	{
		printf("Error: payload: %s\n", res_header.payload);
		close(sd);
		return -1;
	}
	else
	{
		printf("Var %d received\n",id);
		memcpy(data,res_header.payload,res_header.len);
		close(sd);
		return res_header.len;
	}
}