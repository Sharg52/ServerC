#ifndef USER_H
#define USER_H
typedef struct Message
{
	char text[5000];
	char username[100];
	int id;
}Message;
typedef struct Chat
{
	int number_msg;
	Message* messages;
	int first_id;
	int second_id;
}Chat;
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
typedef struct Serv
{
	SOCKET client;
	int number_chats;
	Chat* chats;
	User* users;
	int number_users;
}Serv;
#endif 