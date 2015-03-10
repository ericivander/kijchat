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



typedef struct user
{
	int sockcli;
	char name[200];
	char dest[200];
	int *countUser;
	struct user *prev, *next, **first;
} user;

user *first, *tmp;

int readresponse(int, char *);
pthread_t pt[100];
int currentUser = 0;
user clientList[100];
char buf[1000], respbuf[1000];

void* threadClient(void *arg)
{
	int retval;
	user *client = (user *) arg;
	PRINT("Accept !");
	sprintf(respbuf, "Selamat Datang\r\n");
	retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
	while (retval != -1)
	{
		retval = readresponse(client->sockcli, buf);
		PRINT(buf);
		if (retval == -1)
			break;
		else if (strcasecmp(buf, "QUIT") == 0)
		{
			sprintf(respbuf, "221 Terminating program...\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf), 0);
			retval = -1;
			 
			if (*(client->first) == client)
			    *(client->first) = client->next;
			if (client->next != NULL)
				client->next->prev = client->prev;
			if (client->prev != NULL)
				client->prev->next = client->next;
		}
		else if (strncasecmp(buf, "NAME", 4) == 0)
		{
			sscanf(buf,"%*s %s",client->name);
			//PRINT(client->name);
		}
		else if (strcasecmp(buf, "LIST") == 0)
		{
			user *tmp = client;
			while (tmp->prev != NULL)
				tmp = tmp->prev;
			char text[1000];
			memset(text, 0, sizeof(text));
			while (tmp->next != NULL)
			{
				strcat(text, tmp->name);
				strcat(text, ";");
				tmp = tmp->next;	
			}
			strcat(text, tmp->name);
			strcat(text, ";\r\n");
			retval = send(client->sockcli, text, strlen(text), 0);
		}
		else if (strncasecmp(buf, "RCPT", 4) == 0)
		{
			sscanf(buf, "%*s %s", client->dest);
		}
		else if (strncasecmp(buf, "MSG", 3) == 0)
		{
			user *tmp = client;
			while (tmp->prev != NULL)
				tmp = tmp->prev;
			char text[1000];
			memset(text, 0, sizeof(text));
			while (tmp->next != NULL)
			{
				if (strcasecmp(tmp->name,client->dest) == 0)
					break;
				tmp = tmp->next;	
			}
			int i;
			if (strlen(buf) > 3)
			{
				for (i = 0; i < strlen(buf) - 4; i++)
				{
					text[i] = buf[i + 4];
				}
				strcat(text, "\r\n");
				retval = send(tmp->sockcli, text, strlen(text), 0);
			}
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
		tmp->sockcli = accept(sockfd, (struct sockaddr *)&cliaddr, &clisize);
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
		retval = recv(sockfd, &tmp,1,0);
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
