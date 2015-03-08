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



typedef struct user{
	int sockcli;
	struct sockaddr_in cliaddr;
	int addrlen;
	char name[200];
	char dest[200];
	int *countUser;
	struct user *linkLeft, *linkRight, *first;
}user;

user *first,*last,*tmp;

int readresponse(int, char *);
pthread_t pt[100];
int currentUser = 0;
user clientList[100];
char buf[200], respbuf[200];

void* threadClient(void *arg)
{
	int retval;
	user *client = (user *) arg;
	PRINT("Accept !");
	sprintf(respbuf,"Selamat Datang\r\n");
	retval = send(client->sockcli, respbuf, strlen(respbuf),0);
	while(retval != -1){
		retval = readresponse(client->sockcli, buf);
		PRINT(buf);
		if (retval == -1)
			break;
		else if (strcasecmp(buf, "QUIT") == 0){
			sprintf(respbuf,"221 Terminating program...\r\n");
			retval = send(client->sockcli, respbuf, strlen(respbuf),0);
			retval = -1;
		}
		else if (strncasecmp(buf, "NAME",4) == 0){
			sscanf(buf,"%*s %s",client->name);
			PRINT(client->name);
		}
		else if (strcasecmp(buf, "LIST") == 0){
			//user *temp = client->(*first);
			//PRINT((client->first)->name);
		}
		else{
			sprintf(respbuf,"%d %s\r\n",*client->countUser,buf);
			retval = send(client->sockcli, respbuf, strlen(respbuf),0);
		}
	}
	(*client->countUser)--;
	close(client->sockcli);
}

int main (){
	int sockserv, retval, clisize;

	int sockfd, sockcli, auth = 0;
	struct sockaddr_in servaddr, cliaddr;
	
	sockfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	PRINT("Socket dibuat !");

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	retval = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	PRINT("Port Binding berhasil !");

	retval = listen(sockfd, BACKLOG);


	while (1){
		tmp = (user*)malloc(sizeof(user));
		tmp->cliaddr = cliaddr;
		tmp->addrlen = clisize;
		tmp->sockcli = accept(sockfd,(struct sockaddr*)&cliaddr, &clisize);
		tmp->countUser = &currentUser;
		tmp->first = first;

		if (first == NULL){
			tmp->linkLeft = NULL;
			tmp->linkRight= NULL;
			first = tmp;
			last = tmp;
		}
		else{
			last->linkRight = tmp;
			tmp->linkLeft = last;
			last = tmp;
		}
		pthread_create(&pt[currentUser],NULL,threadClient,last);
		currentUser++;
		tmp = NULL;
		free(tmp);
	}

}

int readresponse(int sockfd, char *buf){
	char tmp;
	int retval, index = 0;

	do{
		retval = recv(sockfd, &tmp,1,0);
		if (retval > 0){
			if (tmp == '\r'){
				recv(sockfd, &tmp,1,0);
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