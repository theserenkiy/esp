#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
	//Создаём сокет
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
	{ 
		printf("ERROR opening socket\n");
		return -1;
	}
	// структура для хранения данных об адресе сервера
	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr("37.46.135.97"),
		.sin_port = htons(8640)
	};	
	//Подключаемся к серверу
	//указатель на serveraddr приводим к типу "struct sockaddr *"
	if(connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		printf("ERROR connecting\n");
		return -1;
	}

    //объявляем два буфера: для отправляемого сообщения и для чтения ответа
    char send_buffer[1024];
    char rcv_buffer[1024];

    printf("Введите сообщение:\n");
    fgets(send_buffer,1024,stdin);

    //пишем в сокет запрос на сервер
    write(sd,send_buffer,1024);

    //читаем из сокета ответ с сервера
    read(sd,rcv_buffer,1024);

    printf("Сервер ответил: %s",rcv_buffer);

    //закрываем соединение с сервером
    close(sd);

}



