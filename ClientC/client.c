// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")

#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include "Cuser.h"
pthread_mutex_t mutex;
pthread_t recv_msg, send_msg;
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
void* recv_serv(User* User2)//получения сообщения
{
	User* User = User2;
	int ret = SOCKET_ERROR;
	//printf("SEERV");
	while (ret == SOCKET_ERROR)
		{
		//printf("TUT\n");
			ret = recv(User->person, User, sizeof(struct User), 0);
			if (ret == 0 || ret == WSAECONNRESET)
			{

				printf("Connection closed\n");
				break;
			}
			if (ret < 0)
			{
				printf("Can't resieve message\n");
				continue;
			}
		}
	
}
int send_serv(User* User2)//отправка сообщения
{
	User* User = User2;
		int ret = send(User->person, User, sizeof(struct User), 0);
		if (ret == SOCKET_ERROR)
		{
			printf("Can't send message\n");
			closesocket(User->person);
			return -1;
		}
		return ret;	
}
void* recv_handler(User* User2)//чтение и печать
{
	User* User = User2;
	//struct User* User;
	
	while (1)
	{
		//printf("rev\n");
		recv_serv(User);
		system("cls");
		printf("%s", User->answer);
		//send_serv(User);
		//printf("STATE=%d", User->state);
	}
	return User;
}
void* send_handler(User* User2)//cканировние и отправка
{
	User* User = User2;
	
	while (1)
	{
		//printf("send\n");
		send_serv(User);
		//printf("STATE=%d\n", User->state);
		switch (User->state)
		{
			case 0:
			{

				scanf("%s", User->password);
				break;
			}
			case 2:
			{
			
				scanf("%d", &User->number);
		
				break;
			}
			case 3:
			{
				scanf("%s", User->message);
				//printf("%s", User->message);
			}
		}
		
	}
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
	User.person = creates_sock();
	struct sockaddr_in server = create_server(User.person);
	pthread_mutex_init(&mutex, NULL);
		if (pthread_create(&send_msg, NULL, send_handler, &User) != 0) 
		{
			printf("ERROR: pthread\n");
			return EXIT_FAILURE;
		}
		if (pthread_create(&recv_msg, NULL, recv_handler, &User) != 0)
		{
			printf("ERROR: pthread\n");
		}
		pthread_join(send_msg, NULL);
		pthread_join(recv_msg, NULL);
	return 0;
}

