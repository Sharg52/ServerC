#ifndef USER_H
#define USER_H
typedef struct User
{
	int chat_id;
	int id;
	char username[100];
	char password[100];
	char message[2000];
	char answer[2000];
	int state;
	int number;
	SOCKET person;
}User;
#endif 