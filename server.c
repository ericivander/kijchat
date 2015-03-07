#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define PORT 6060
#define BACKLOG 10
#define PRINT(msg) printf ("%s\n", msg)

int readresponse(int, char *);

int main ()
{
	int sockserv, retval, clisize;

	int sockfd, sockcli, auth = 0;
	struct sockaddr_in servaddr, cliaddr;
	char buf[200], respbuf[200], user[200], pass[200];
	
	sockfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	PRINT("Socket dibuat !");

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	retval = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	PRINT("Port Binding berhasil !");

	retval = listen(sockfd, BACKLOG);

	sockcli = accept(sockfd,(struct sockaddr*)&cliaddr, &clisize);
	PRINT("Accept !");
	sprintf(respbuf,"Selamat Datang\r\n");
	retval = send(sockcli, respbuf, strlen(respbuf),0);
	while(retval != -1){
		retval = readresponse(sockcli, buf);
		PRINT(buf);
		if (retval == -1)
			break;
		else if (strcasecmp(buf, "QUIT") == 0){
			sprintf(respbuf,"221 Terminating program...\r\n");
			retval = send(sockcli, respbuf, strlen(respbuf),0);
			retval = -1;
		}
		else{
			PRINT("Bad Request");
			sprintf(respbuf,"%s\r\n",buf);
			retval = send(sockcli, respbuf, strlen(respbuf),0);
		}
	}
}

int readresponse(int sockfd, char *buf){
	char tmp;
	int retval, index = 0;

	do{
		retval = recv(sockfd, &tmp,sizeof(char), 0);
		if (retval > 0){
			if (tmp == '\r'){
				recv(sockfd, &tmp,sizeof(char), 0);
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