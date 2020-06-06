// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include "Suser.h"
#define max_user 50


pthread_mutex_t mutex;
int check_user(User User)
{

}
void* ClientStart(void* param)
{
	Serv* Serv = param;
	struct User User;
	int ret;
	ret = recv(Serv->client, &User, sizeof(User), 0);
	if (!ret || ret == SOCKET_ERROR)
	{
		pthread_mutex_lock(&mutex);
		printf("Error getting data\n");
		pthread_mutex_unlock(&mutex);
		return (void*)1;
	}
	pthread_mutex_lock(&mutex);
	switch (User.state)
	{
	case 0:
	{
		int flag = -1;
		for (int i = 0; i < Serv->number_users; i++)
		{
			if (strcmp(User.username, Serv->users[i].username) == 0)
			{
				flag = i;
				User.id = flag;
				break;
			}
		}

		if (flag == -1)
		{
			flag = Serv->number_users;
			Serv->number_users++;
			User.id = flag;
			Serv->users[flag].id = flag;
			strcpy(Serv->users[flag].username, User.username);
			strcpy(Serv->users[flag].password, User.password);
			User.state = 1;
			Serv->users[flag].state = 1;
			strcpy(User.answer,"Welcome to our chat");
		}
		else
		{
			strcpy(User.answer,"Welcome back");
			User.state = 1;
		}

		if (strcmp(Serv->users[flag].password, User.password) != 0)
		{

			User.state = 0;

		}
		break;
	}
	case 1:
	{
		strcpy(User.answer,"\n");
		for (int i = 0; i < Serv->number_users; i++)
		{
			if (User.id!=Serv->users[i].id)
			{
				Serv->users[User.id].chats[i].number_msg=0;
				User.chats->number_msg = 0;
				char temp[100*max_user];
				sprintf(temp, "%d%s %s\n", Serv->users[i].id, ". ", Serv->users[i].username);
				strcat(User.answer, temp);
			}
		}
		if (strcmp(User.answer, "\n")==0)
			strcpy(User.answer, "You are the only user");
		User.state = 2;
		break;
	}
	case 2:
	{
		if (User.id != User.number && User.number >= 0 && User.number < Serv->number_users)
		{
			sprintf(User.answer, "%s %s\n","You are in a chat with", Serv->users[User.number].username);
			User.state = 3;
			for (int i = 0; i < User.chats[User.number].number_msg; i++)
			{
				char temp[100000];
				sprintf(temp, "%d%s %s %s\n", Serv->users[i].id, ". ", Serv->users[i].username, User.chats[User.number].chat[i]);
				strcat(User.answer, temp);
			}
		}
		else
			strcpy(User.answer, "Id incorrect");
		break;
	}
	case 3:
	{
		strcpy(Serv->users[User.id].chats[User.number].chat[Serv->users[User.id].chats[User.number].number_msg], User.chats[User.number].chat[User.chats[User.number].number_msg]);
		//sprintf(User.chats[User.number].chat[User.chats[User.number].number_msg], "%d%s %s %s\n", Serv->users[i].id, ". ", Serv->users[i].username, User.chats[User.number].chat[i]);
	}
	}
	printf("%s\n", User.username);
	pthread_mutex_unlock(&mutex);
	
	ret = send(Serv->client, &User, sizeof(User), 0);
	if (ret == SOCKET_ERROR)
	{
		pthread_mutex_lock(&mutex);
		printf("Error sending data\n");
		pthread_mutex_unlock(&mutex);
		return (void*)2;
	}
	return (void*)0;
}

int CreateServer()
{
	struct User User;
	struct Serv Serv2;
	struct Serv *Serv;
	struct User* users = malloc(max_user * sizeof(User));
	Serv = &Serv2;
	Serv->users = users;
	Serv->number_users = 0;
	SOCKET server;
	struct sockaddr_in localaddr, clientaddr;
	int size;
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (server == INVALID_SOCKET)
	{
		printf("Error create server\n");
		return 1;
	}
	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(5510);//port number is for example, must be more than 1024
	if (bind(server, (struct sockaddr*) & localaddr, sizeof(localaddr)) == SOCKET_ERROR)
	{
		printf("Can't start server\n");
		return 2;
	}
	else
	{
		printf("Server is started\n");
	}
	
	listen(server, max_user);//max_user клиентов в очереди могут стоять
	pthread_mutex_init(&mutex, NULL);
	while (1)
	{
		size = sizeof(clientaddr);
		Serv->client = accept(server, (struct sockaddr*) & clientaddr, &size);
		if (Serv->client == INVALID_SOCKET)
		{
			printf("Error accept client\n");
			continue;
		}
		pthread_t mythread;
		int status = pthread_create(&mythread, NULL, ClientStart, (void*)Serv);
		pthread_detach(mythread);
	}
	pthread_mutex_destroy(&mutex);
	printf("Server is stopped\n");
	closesocket(server);
	return 0;
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0)
	{
		printf("Connected to socket lib\n");
	}
	else
	{
		return 1;
	}
	int err = 0;
	if (err = CreateServer())
	{
		return err;
	}
	return 0;
}

