#include <stdio.h>

int main()
{
    //��।��塞 ⨯ user_t
	typedef struct user {
		char name[16];	   		//���
		int birth_year; 		//��� ஦�����
	} user_t;

	user_t user;			    //���塞 ��६����� ⨯� user_t

	//������塞 ����묨 �������� user
    printf("������ ��� ���짮��⥫�: ");
    scanf("%15s", user.name);
    printf("������ ��� ஦�����: ");
    scanf("%d", &user.birth_year);

    //���뢠�� 䠩� users.dat � ०��� ��ᮥ�������
	FILE *fp = fopen("users.dat", "a");

    //�஢��塞 �� 䠩� ������
    if(!fp)
    {
        printf("�訡�� ����㯠 � 䠩�� ������\n");
        return -1;
    }
    
    //��襬 � 䠩� ����� �� ����� �� 㪠��⥫� �� ��६����� user
    //ࠧ��� ������ ����� = ࠧ��� ����� �������� ⨯� user_t
    //������⢮ �����뢠���� ����⮢ ������� ⨯� = 1
    fwrite(&user,sizeof(user_t),1,fp);

    //����뢠�� 䠩�
    fclose(fp);
	return 0;
}

