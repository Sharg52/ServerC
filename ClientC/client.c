// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Cuser.h"
#define max_user 50
SOCKET creates_sock()
{
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	return client;
}
struct sockaddr_in create_server(SOCKET sock)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(sock, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(sock);
		return;
	}
	return server;
}
int SendData2Server(User* User2, struct sockaddr_in server)
{
	while (1)
	{
		User* User = User2;
		int ret;
		ret = send(User->person, User, sizeof(struct User), 0);
		if (ret == SOCKET_ERROR)
		{
			printf("Can't send message\n");
			//closesocket(User->person);
			//return 1;
		}
		ret = SOCKET_ERROR;
			//полчение ответа
			ret = recv(User->person, User, sizeof(struct User), 0);
			printf("State%d\n", User->state);
			//обработка ошибок
			if (ret == 0 || ret == WSAECONNRESET)
			{
				printf("Connection closed\n");
				//break;
			}
			if (ret < 0)
			{
				printf("Can't resieve message\n");
				//closesocket(User->person);
				//return;
				continue;
			}
			switch (User->state)
			{
			case 0:
			{

				printf("Password incorrect, please try now\n");
				scanf("%s", User->password);
				//closesocket(User->person);
				//return 0;
				break;
			}
			case 1:
			{
				
				printf("%s\n", User->answer);
				printf("Enter the number with whom you want to talk\n");
				//return User->state;
				break;
			}
			case 2:
			{
				printf("%s\n", User->answer);
				scanf("%d", &User->number);
				//return User->state;
				break;
			}
			case 3:
			{
				printf("%s", User->answer);
				gets(User->message);
				//return User->state;
				break;
			}
			}
			//вывод на экран количества полученных байт и сообщение
	}
	//closesocket(User->person);
	return 1;
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Can't connect to socket lib");
		return 1;
	}
	int state = 3;
	printf("Enter your username:\n");
	struct User User;
	struct User* Userptr;
	Userptr = &User;
	scanf("%s", User.username);
	printf("Enter your password:\n");
	scanf("%s", User.password);
	User.state = 0;
	int exit = 1;
	User.person =creates_sock();
	while (exit)
	{
		struct sockaddr_in server = create_server(User.person);
		User.state = SendData2Server(&User,server);
		
	}
	return 0;
}

