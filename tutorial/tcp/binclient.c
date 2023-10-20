#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

#define HOST_IP "127.0.0.1"
#define HOST_PORT 1337

#define TOKEN ""

typedef struct {
	char name[32];
	int age;
}nigga_t;

typedef struct {
	int status;
	int length;
} response_header_t;

int main(int argc, char **argv) 
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
		printf("ERROR opening socket\n");

	//Подключаемся к серверу
	if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		printf("ERROR connecting\n");

	//Пишем в сокет (отправляем сообщение на сервер)
	int nbytes;
	
	nbytes = write(sockfd, TOKEN, strlen(TOKEN));
	if (nbytes < 0) 
		printf("ERROR writing header to socket\n");

	close(sockfd);
	return 0;

	char buf[BUFSIZE];
	response_header_t header;
	//Читаем из сокета (принимаем сообщение с сервера)
	nbytes = read(sockfd, &header, sizeof(response_header_t));
	if (nbytes < 0) 
		printf("ERROR reading from socket\n");

	printf("Response header: status=%d, length=%d\n", header.status, header.length); 

	//nigga = (nigga_t*)(buf + (nbytes-sizeof(nigga_t)));

	//printf("Nigga name: %s, nigga age: %d\n", nigga->name, nigga->age);

	//закрываем соединение
	close(sockfd);
	return 0;
}
