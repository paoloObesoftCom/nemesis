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
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char *strCmd;
	char buffer[256];
	if (argc < 4)
	{	fprintf(stderr,"usage %s hostname port command\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{	close(sockfd);
		error("ERROR connecting");
	}	
	strCmd  = argv[3];
	n = write(sockfd,strCmd,strlen(strCmd));
	if (n < 0) 
	{	close(sockfd);
		error("ERROR write");
	}
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0) 
	{	close(sockfd);
		error("ERROR read");
	}
	close(sockfd);
	return 0;
}

