// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include "Suser.h"
#define max_user 20
#define max_messages 100
#define max_user_online 20

pthread_mutex_t mutex;
int check_user(User User)
{

}
void* ClientStart(void* param)
{
	while(1)
	{
	
	Serv* Serv = param;
	struct User User;
	int ret;
	ret = recv(Serv->client, &User, sizeof(User), 0);

	if (!ret || ret == SOCKET_ERROR)
	{
		printf("Error getting data\n");
		return (void*)1;
	}
	pthread_mutex_lock(&mutex);
	//printf("\nID %d\n", User.id);
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
			printf("User: %s create\n", User.username);
			strcpy(User.answer, "Welcome to our chat");
			Serv->users[flag].person = Serv->client;
		}
		else
		{
			Serv->users[flag].person = Serv->client;
			strcpy(User.answer, "Welcome back");
			User.state = 1;
		}

		if (strcmp(Serv->users[flag].password, User.password) != 0)
		{
			strcpy(User.answer,"Password incorrect try now\n");
			User.state = 0;

		}
		break;
	}
	case 1:
	{
		strcpy(User.answer, "Choose a user number\n");
		int flag = 0;
		for (int i = 0; i < Serv->number_users; i++)
		{
			if (User.id != Serv->users[i].id)
			{
				printf("FLag=%d\t",flag);
				char temp[100 * max_user];
				sprintf(temp, "%d%s %s\n", Serv->users[i].id, ". ", Serv->users[i].username);
				strcat(User.answer, temp);
				flag = 1;
			}
		}
		if (flag == 0)
			strcpy(User.answer, "You are the only user\n");
		else
		{
			printf("%s", User.answer);
			User.state = 2;
		}
		break;
	}
	case 2:
	{
		if (User.id != User.number && User.number >= 0 && User.number < Serv->number_users)
		{
			sprintf(User.answer, "%s %s\n", "You are in a chat with", Serv->users[User.number].username);
			User.state = 3;
			User.chat_id = -1;
			for (int i = 0; i < Serv->number_chats; i++)
			{
				if ((Serv->chats[i].first_id == User.id && Serv->chats[i].second_id == User.number) || (Serv->chats[i].second_id == User.id && Serv->chats[i].first_id == User.number))
				{
					User.chat_id = i;
					break;
				}
			}
			if (User.chat_id == -1)
			{

				User.chat_id = Serv->number_chats;
				Serv->chats[User.chat_id].number_msg = 0;
				Serv->chats[User.chat_id].first_id = User.id;
				Serv->chats[User.chat_id].second_id = User.number;
				struct message* messages = malloc(max_messages * sizeof(Message));
				Serv->chats[User.chat_id].messages = messages;
				Serv->number_chats++;
			}
			int n = Serv->chats[User.chat_id].number_msg;
			printf("%d", n);
			for (int i = 0; i < n; i++)
			{
				char temp[2000];
				//printf("%s", Serv->chats[User.chat_id].messages[i].text);
				sprintf(temp, "%s:\n %s\n", Serv->chats[User.chat_id].messages[i].username, Serv->chats[User.chat_id].messages[i].text);
				strcat(User.answer, temp);
			}
		}
		else
			strcpy(User.answer, "Id incorrect");
		break;
	}
	case 3:
	{
		strcpy(User.answer, "");
		strcpy(Serv->chats[User.chat_id].messages[Serv->chats[User.chat_id].number_msg].text, User.message);
		Serv->chats[User.chat_id].messages[Serv->chats[User.chat_id].number_msg].id = User.id;
		strcpy(Serv->chats[User.chat_id].messages[Serv->chats[User.chat_id].number_msg].username, User.username);
		Serv->chats[User.chat_id].number_msg++;
		strcpy(Serv->users[User.number].answer, User.message);
		printf("%d %s\n", User.number, Serv->users[User.number].answer);
		//pthread_mutex_unlock(&mutex);
		ret = send(Serv->users[User.number].person, &Serv->users[User.number], sizeof(User), 0);
		//pthread_mutex_lock(&mutex);
		if (ret == SOCKET_ERROR)
		{
			pthread_mutex_lock(&mutex);
			printf("!!!Error sending data\n");
			pthread_mutex_unlock(&mutex);
			return (void*)2;
		}
		break;
	}
	}
	pthread_mutex_unlock(&mutex);
	ret = send(Serv->users[User.id].person, &User, sizeof(User), 0);
	
	if (ret == SOCKET_ERROR)
	{
		pthread_mutex_lock(&mutex);
		printf("Error sending data\n");
		pthread_mutex_unlock(&mutex);
		return (void*)2;
	}
	printf("State%d\n", User.state);
}
	return (void*)0;
}

int CreateServer()
{
	struct User User;
	struct Chat Chat;
	struct Serv Serv2;
	struct Serv *Serv;
	struct User* users = malloc(max_user * sizeof(User));
	struct Chat* chats = malloc(max_user*max_user * sizeof(Chat));
	Serv = &Serv2;
	Serv->users = users;
	Serv->chats = chats;
	Serv->number_chats = 0;
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
		Serv->client= accept(server, (struct sockaddr*) & clientaddr, &size);
		if (Serv->client == INVALID_SOCKET)
		{
			printf("Error accept client\n");
			continue;
		}
		pthread_t mythread;
		int status = pthread_create(&mythread, NULL, ClientStart, (void*)Serv);
		//pthread_join(mythread, (void**)NULL);
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

