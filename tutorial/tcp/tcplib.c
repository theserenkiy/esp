#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BUFSIZE 1024

typedef struct{
	int port;
	char *method;
	char *url;
	char *headers;
	void *body;
	int body_length;
	void *result;
} http_fetch_config;

http_fetch_config http_fetch_config_default = {
	.port = 80,
	.method = "GET",
	.url = "/",
	.headers = "",
	.body = NULL,
	.body_length = 0,
	.result = NULL
};

int lowercase(char *str) {
	for(int i = 0; i < 100; i++)
	{
		if(str[i] == 0)
			break;
		if(str[i] >= 'A' && str[i] <= 'Z')
			str[i] += ('a'-'A');
	}
}

int http_fetch(char *ip, http_fetch_config *c)
{
	// идентификатор сокета
	int sockfd;			

	if(!c)c = &http_fetch_config_default;

	if(!c->method)
		c->method = http_fetch_config_default.method;
	
	if(!c->port)
		c->port = http_fetch_config_default.port;

	if(!c->url)
		c->url = http_fetch_config_default.url;

	if(!c->headers)
		c->headers = http_fetch_config_default.headers;

	printf("%s %s:%d%s ...\n", c->method, ip, c->port, c->url);

	//сообщение которое отправим на сервер
	char head[1024];
	char contentLengthHeader[32] = {0};
	if(c->body)
	{
		sprintf(contentLengthHeader,"Content-Length: %d\r\n", c->body_length);
	}
	sprintf(head,"%s %s HTTP/1.1\r\n%s%sConnection: close\r\n\r\n",c->method,c->url,c->headers,contentLengthHeader);
	
	//printf("Head: %s\n",head);

	// буфер, куда будем читать
	char buf[BUFSIZE];
	
	// структура для хранения данных об адресе сервера
	struct sockaddr_in serveraddr;	

	//инициализация адреса сервера (куда подключаемся)
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(c->port);

	//Создаём сокет
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		printf("ERROR opening socket\n");

	//Подключаемся к серверу
	if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		printf("ERROR connecting\n");
		return -1;
	}

	//Пишем в сокет HTTP-хедер
	int nbytes;
	nbytes = write(sockfd, head, strlen(head));
	if (nbytes < 0) 
	{
		printf("ERROR writing head to socket\n");
		return -1;
	}
	
	//Пишем в сокет тело запроса
	if(c->body)
	{
		nbytes = write(sockfd, c->body, c->body_length);
		if (nbytes < 0) 
		{
			printf("ERROR writing body to socket\n");
			return -1;
		}
	}

	char header[1024];
	char *data, *dataPtr, *bufStart;
	int bytesToWrite;
	//Читаем из сокета (принимаем сообщение с сервера)
	for(int i=0;;i++)
	{
		nbytes = read(sockfd, buf, BUFSIZE);
		printf("nbytes %d\n", nbytes);
		if (nbytes < 0) 
		{
			printf("ERROR reading from socket\n");
			return -1;
		}

		if(i==0)
		{
			char *cl = strstr(buf, "Content-Length:");
			int datalen;
			if(!cl)
			{
				printf("ERROR Content-Length header is missing\n");
				return -1;
			}
			
			sscanf(cl+15, "%d", &datalen);
			printf("Content-Length: %d\n",datalen);
			data = malloc(datalen);
			dataPtr = data;

			bufStart = strstr(buf,"\r\n\r\n");
			if(!bufStart)
			{
				printf("ERROR End of HTTP header not found\n");
				return -1;
			}

			printf("memcpy header length: %d\n",bufStart-buf);
			memcpy(header,buf,bufStart-buf);
			
			bytesToWrite = buf+nbytes-bufStart-4;
		}
		else{
			bufStart = buf;
			bytesToWrite = nbytes;
		}

		if(!nbytes)
			break;

		printf("memcpy data length: %d\n",bytesToWrite);
		memcpy(dataPtr, bufStart, bytesToWrite);
		dataPtr += bytesToWrite;
	}
	

	printf("Response header:\n%s\n\nResponse body:\n%s\n\n", header, data); 

	//закрываем соединение
	close(sockfd);
}

int http_get(char *ip, int port, char *url)
{
	http_fetch_config c = {0};

	c.port = port;
	c.url = url;

	return http_fetch(ip,&c);
}