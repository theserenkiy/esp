#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

#define HOST_IP "127.0.0.1"
#define HOST_PORT 5555

typedef struct {
	char name[32];
	int age;
}nigga_t;


int main(int argc, char **argv) 
{
	// идентификатор сокета
	int sockfd;			

	//сообщение которое отправим на сервер
	char header[1024];

	nigga_t nigga = {
		.name = "nigga",
		.age = 22
	};

	sprintf(header, "POST /?id=123 HTTP/1.1\r\nContent-length: %d\r\nConnection: close\r\n\r\n", sizeof(nigga_t));

	// буфер, куда будем читать
	char buf[BUFSIZE];	
	
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
	nbytes = write(sockfd, header, strlen(header));
	if (nbytes < 0) 
		printf("ERROR writing header to socket\n");
	nbytes = write(sockfd, &nigga, sizeof(nigga_t));
	if (nbytes < 0) 
		printf("ERROR writing payload to socket\n");

	//Читаем из сокета (принимаем сообщение с сервера)
	nbytes = read(sockfd, buf, BUFSIZE);
	if (nbytes < 0) 
		printf("ERROR reading from socket\n");

	printf("Read from server %d bytes:\n%s", nbytes, buf); 

	//закрываем соединение
	close(sockfd);
	return 0;
}
