// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Cuser.h"
int SendData2Server(User* User2)
{
	User* User = User2;
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}

	int ret = send(client,User, sizeof(struct User), 0);

	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	ret = SOCKET_ERROR;
	while (ret == SOCKET_ERROR)
	{

		//полчение ответа
		ret = recv(client, User, sizeof(struct User), 0);
		printf("State=%d\n", User->state);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			printf("Can't resieve message\n");
			closesocket(client);
			return;
			continue;
		}
		switch (User->state)
		{
		case 0:
		{
			closesocket(client);
			return 0;
		}
		case 1:
		{
			printf("%s\n", User->answer);
			return User->state;
		}
		case 2:
		{
			printf("%s\n", User->answer);
			return User->state;
		}
		case 3:
		{
			printf("%s\n", User->answer);
			return User->state;
		}
		}
		//вывод на экран количества полученных байт и сообщение
		
	}

	closesocket(client);
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
	scanf("%s", User.username);
	printf("Enter your password:\n");
	scanf("%s", User.password);
	User.state = 0;
	int exit = 1;
	while (exit)
	{
		User.state = SendData2Server(&User);
		if (User.state == 0)
		{
			printf("Password incorrect, please try now\n");
			scanf("%s", User.password);
		}
		if (User.state == 1)
		{
			printf("Enter the number with whom you want to talk\n");
			
		}
		if (User.state==2)
			scanf("%d", &User.number);
		if (User.state == 3)
			scanf("%s", User.chats[User.number].chat[User.chats[User.number].number_msg]);
	}
	return 0;
}

