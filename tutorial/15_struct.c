#include <stdio.h>

int main()
{
	struct user {
		char name[128];	   		//имя
		char gender;			//пол
		short birth_year; 		//год рождения
	};

	struct user user1;

	printf("Введите имя: ");
	scanf("%128s", user1.name);
	
	printf("Введите пол: ");
	scanf(" %c", &user1.gender);
	
	printf("Введите год рождения: ");
	scanf("%d", &user1.birth_year);

	printf("%s, %c, %d г.р\n", user1.name, user1.gender, user1.birth_year);

	return 0;
}


