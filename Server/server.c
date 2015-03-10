#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 6060
#define BACKLOG 10
#define PRINT(msg) printf ("%s\n", msg)

/*
	1xx Authentification
	100 Username Set
	101 Username Not Set
	102 Username Already Taken
	2xx Connection
	200 Connection Success
	201 Logging Out
	4xx Response Message
	400 LIST Response
	401 MSG Response
	5xx Command
	500 Unknown Command
*/

// Node for each client
typedef struct user
{
	int sockcli;
	char name[200];
	char dest[200];
	int *countUser;
	struct user *prev, *next, **first;
} user;

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
			
			// Broadcast current LIST to all online user
			user *tmp = *(client->first);
			char text[1000];
			memset(text, 0, sizeof(text));
			strcat(text, "400#");
			while (tmp != NULL)
			{
				if(strcmp(tmp->name, "Unknown") != 0)
				{
					strcat(text, tmp->name);
					strcat(text, ";");
				}
				tmp = tmp->next;
			}
			strcat(text, "\r\n");
			tmp = *(client->first);
			while (tmp != NULL)
			{
				if(strcmp(tmp->name, "Unknown") != 0)
				{
					send(tmp->sockcli, text, strlen(text), 0);
				}
				tmp = tmp->next;
			}
		}
		// Client setting username for chatting
		// Duplicate username will be forced to close
		else if (strcmp(client->name, "Unknown") == 0 && strncasecmp(buf, "NAME", 4) == 0)
		{
			int taken = 0;
			char name[200];
			sscanf(buf, "%*s %s", name);
			//PRINT(name);
			user *tmp = *(client->first);
			while (tmp != NULL)
			{
				if (strcmp(tmp->name, name) == 0)
				{
					sprintf(respbuf, "102#Username Already Taken\r\n");
					taken = 1;
					send(client->sockcli, respbuf, strlen(respbuf), 0);
					break;
				}
				tmp = tmp->next;
			}
			if (taken == 0)
			{
				sscanf(name, "%s", client->name);
				sprintf(respbuf, "100#Username Set\r\n");
				retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
			}
		}
		// Broadcast current LIST to all online user
		else if (strcmp(client->name, "Unknown") != 0 && strcasecmp(buf, "LIST") == 0)
		{
			user *tmp = *(client->first);
			char text[1000];
			memset(text, 0, sizeof(text));
			strcat(text, "400#");
			while (tmp != NULL)
			{
				if(strcmp(tmp->name, "Unknown") != 0)
				{
					strcat(text, tmp->name);
					strcat(text, ";");
				}
				tmp = tmp->next;
			}
			strcat(text, "\r\n");
			tmp = *(client->first);
			while (tmp != NULL)
			{
				if(strcmp(tmp->name, "Unknown") != 0)
				{
					send(tmp->sockcli, text, strlen(text), 0);
				}
				tmp = tmp->next;
			}
			//retval = send(client->sockcli, text, strlen(text), 0);
		}
		// Set recipient of client's message
		else if (strcmp(client->name, "Unknown") != 0 && strncasecmp(buf, "RCPT", 4) == 0)
		{
			sscanf(buf, "%*s %s", client->dest);
		}
		// Send message to previously set recipient
		else if (strcmp(client->name, "Unknown") != 0 && strncasecmp(buf, "MSG", 3) == 0)
		{
			user *tmp = *(client->first);
			char text[1000];
			memset(text, 0, sizeof(text));
			strcat(text, "401#");
			strcat(text, client->name);
			strcat(text, "#");
			while (tmp != NULL)
			{
				if (strcasecmp(tmp->name, client->dest) == 0)
					break;
				tmp = tmp->next;	
			}
			int i;
			if (strlen(buf) > 3)
			{
				int l = strlen(text);
				for (i = l; i < l + strlen(buf) - 4; i++)
				{
					text[i] = buf[i - l + 4];
				}
				strcat(text, "\r\n");
				retval = send(tmp->sockcli, text, strlen(text), 0);
			}
		}
		// 101 Username Not Set when invoke a command before logged in
		else if (strcmp(client->name, "Unknown") == 0) 
		{
			sprintf(respbuf, "101#Username Not Set\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
		}
		// 500 Unknown Command sent by user
		else
		{
			sprintf(respbuf, "500#Unknown Command\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
		}
	}
	(*client->countUser)--;
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


	while (1)
	{
		tmp = (user *)malloc(sizeof(user));
		// Set new user data, set at head (first) node
		tmp->sockcli = accept(sockfd, (struct sockaddr *)&cliaddr, &clisize);
		strcpy(tmp->name, "Unknown");
		tmp->countUser = &currentUser;
		tmp->first = &first;
		tmp->prev = NULL;
		tmp->next = first;
		if (first != NULL)
			first->prev = tmp;
		first = tmp;
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
