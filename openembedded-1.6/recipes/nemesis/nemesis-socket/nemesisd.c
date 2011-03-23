 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team
 ****************************************************************************************************
 */

#include <errno.h> 
#include <string.h> 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h> 

void dostuff(int);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void readPortNumber(char * port)
{
	char buf[256];
	FILE *in = fopen("/var/etc/nemesis.cfg", "rt");
	if(in)
	{	
		while (fgets(buf, 256, in))
		{
			if (strstr(buf ,"daemon_port="))
			{	
				fclose(in);
				char * pch;
				pch = strtok(buf,"=");
				pch = strtok(NULL,"=");
				strncpy(port,pch,strlen(pch));
				return;
			}
		}
		fclose(in);
	}
	strncpy(port,"1888",4);
	return;
}

int main()
{
	int sockfd, newsockfd, portno, pid;
	char port[6] = "";
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
		return -1;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	readPortNumber(port);
	portno = atoi(port);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
		return -1;
	}
	
 	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	signal(SIGCHLD,SIG_IGN);

	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		pid = fork();
		if (pid < 0) 
			error("ERROR on fork");
		else if (pid == 0) {
			close(sockfd);
			dostuff(newsockfd);
			exit(0);
		} 
		else close(newsockfd);
	}
	close(sockfd);
	return 0;
}

void dostuff (int sock)
{
	int n;
	char buffer[256];
	
	bzero(buffer,256);
	n = read(sock,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	system(buffer);
	//printf("Here is the message: %s\n",buffer);
	n = write(sock,"OK",2);
	if (n < 0) error("ERROR writing to socket");
}

