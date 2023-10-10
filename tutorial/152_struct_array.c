#include <stdio.h>

int main()
{
	struct user {
		char name[128];	   		//имя
		char gender;			//пол
		short birth_year; 		//год рождения
	};

	typedef struct user user_t;	//определяем тип user_t
	user_t users[4];			//объявляем массив из 4 элементов user_t

	//заполняем данными структуры - элементы массива
	for(int i = 0; i < 4; i++)
	{
		printf("Введите имя пользователя #%d: ",i+1);
		scanf("%128s", users[i].name);
		
		printf("Введите пол: ");
		scanf(" %c", &users[i].gender);
		
		printf("Введите год рождения: ");
		scanf("%d", &users[i].birth_year);
	}

	//перебираем структуры в массиве - выводим данные 
	for(int i=0; i < 4;i++)
	{
		printf("%s, %c, %d г.р\n", 
			users[i].name, 
			users[i].gender, 
			users[i].birth_year
		);
	}
	return 0;
}


