#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <map>
#include <pthread.h>

using namespace std;

#define PORT 6161
#define BACKLOG 10
#define PRINT(msg) printf ("%s\n", msg)



/*
	1xx Authentification
	100 Username Set
	2xx Connection
	200 Connection Success
	201 Logging Out
	4xx Response Message
	400 public key of client
	401 client not found
*/

// Node for each client
typedef struct user
{
	int sockcli;
	char name[200];
	struct user *prev, *next, **first;
} user;

map<string, string> dict;

// Pointer to head and temporary var
user *first, *tmp;

// Function Declaration : to readresponse from client
int readresponse(int, char *);

pthread_t pt[100];
int currentUser = 0;
user clientList[100];
char buf[1000], respbuf[1000];

// Client's thread
void* threadClient(void *arg)
{
	int retval = 0;
	user *client = (user *) arg;
	PRINT("Accept !");
	// After each successful connection
	sprintf(respbuf, "200#Connect Success\r\n");
	retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
	while (retval != -1)
	{
		retval = readresponse(client->sockcli, buf);
		PRINT(buf);
		if (retval == -1)
			break;
		// Client QUIT from the chat
		else if (strcasecmp(buf, "QUIT") == 0)
		{
			sprintf(respbuf, "201#Logging Out\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
			retval = -1;
			
			if (*(client->first) == client)
			    *(client->first) = client->next;
			if (client->next != NULL)
				client->next->prev = client->prev;
			if (client->prev != NULL)
				client->prev->next = client->next;
			
			dict[client->name] = "";
		}
		// Save client username
		else if (strncasecmp(buf, "NAME", 4) == 0)
		{
			char name[200];
			memset(name, 0, sizeof(name));
			sscanf(buf, "%*s %s", name);
			//PRINT(name);
			strcpy(client->name, name);
			sprintf(respbuf, "100#Username Set\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
		}
		// Set public key of client's
		else if (strncasecmp(buf, "SET", 3) == 0)
		{
			char key[1024];
			memset(key, 0, sizeof(key));
			sscanf(buf, "%*s %s", key);
			dict[client->name] = key;
		}
		// Send public key of recipient
		else if (strncasecmp(buf, "GET", 3) == 0)
		{
			char target[1024];
			memset(target, 0, sizeof(target));
			sscanf(buf, "%*s %s", target);
			if(dict[target].length() == 0)
			{
				sprintf(respbuf, "400#Client Not Found\r\n");
				retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
			}
			else
			{
				char text[2048];
				strcat(text, "401#");
				strcat(text, dict[target].c_str());
				strcat(text, "#");
				retval = send(client->sockcli, text, strlen(text), 0);
			}
		}
	}
	close(client->sockcli);
	client = NULL;
	free(client);
}

int main ()
{
	int sockserv, retval, clisize;

	int sockfd, sockcli, auth = 0;
	struct sockaddr_in servaddr, cliaddr;
	
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	PRINT("Socket dibuat !");

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	retval = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	PRINT("Port Binding berhasil !");

	retval = listen(sockfd, BACKLOG);
	PRINT("1");
	while (1)
	{
		PRINT("a");
		tmp = (user *)malloc(sizeof(user));
		// Set new user data, set at head (first) node
		tmp->sockcli = accept(sockfd, (struct sockaddr *)&cliaddr, &clisize);
		PRINT("b");
		strcpy(tmp->name, "");
		PRINT("c");
		tmp->first = &first;
		PRINT("d");
		tmp->prev = NULL;
		PRINT("e");
		tmp->next = first;
		PRINT("f");
		if (first != NULL)
			first->prev = tmp;
		first = tmp;
		PRINT("g");
		PRINT(tmp->name);
		PRINT("h");
		pthread_create(&pt[currentUser], NULL, threadClient, tmp);
		currentUser++;
	}
}

int readresponse(int sockfd, char *buf)
{
	char tmp;
	int retval, index = 0;

	do
	{
		retval = recv(sockfd, &tmp, 1, 0);
		if (retval > 0)
		{
			if (tmp == '\r')
			{
				recv(sockfd, &tmp, 1, 0);
				buf[index] = '\0';
				break;
			}
			else
				buf[index++] = tmp;
		}
	} while (retval > 0);
	if (retval == -1)
		return -1;
	else
		return 0;
}
