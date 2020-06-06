#ifndef USER_H
#define USER_H
typedef struct Chat
{
	int number_msg;
	char chat[1000][10000];
	int second_id;
}Chat;
typedef struct User
{
	Chat* chats;
	int id;
	char username[100];
	char password[100];
	char message[2000];
	char answer[2000];
	int state;
	int number;
}User;
#endif 