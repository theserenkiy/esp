#include <stdio.h>

int main()
{
    //определяем тип user_t
	typedef struct user {
		char name[16];	   		//имя
		int birth_year; 		//год рождения
	} user_t;

	user_t user;			    //объявляем переменную типа user_t

	//заполняем данными структуру user
    printf("Введите имя пользователя: ");
    scanf("%15s", user.name);
    printf("Введите год рождения: ");
    scanf("%d", &user.birth_year);

    //открываем файл users.dat в режиме присоединения
	FILE *fp = fopen("users.dat", "a");

    //проверяем что файл открылся
    if(!fp)
    {
        printf("Ошибка доступа к файлу данных\n");
        return -1;
    }
    
    //пишем в файл данные из памяти по указателю на переменную user
    //размер одного элемента = размер одной структуры типа user_t
    //количество записываемых элементов данного типа = 1
    fwrite(&user,sizeof(user_t),1,fp);

    //закрываем файл
    fclose(fp);
	return 0;
}

