#include "request.h"
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static void sig_chld(int signo);
static void sig_chld(int signo) 
{ 
    pid_t   pid; 
    int     stat; 
			    
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {   
        printf("child %d terminated\n", pid); 
    }   
				    
    return; 
} 
int main(int argc, char **argv)
{
	signal(SIGCHLD, sig_chld);
	int listenfd, connfd, port;
	socklen_t  clientlen;
	struct sockaddr_in clientaddr;
	request_t *q=(request_t *)malloc(sizeof(request_t));

	if(argc <2)
	{
		fprintf(stderr, "no enough arguments!\n for %s\n,plz use %s <port> to run the program\n",argv[0],argv[0]);
		exit(1);
	}

	port = atoi(argv[1]);

	if((listenfd = open_listenfd(port))<0)
	{
		fprintf(stderr,"open listen sock failed!\n");
		exit(1);
	}

	while(1)
	{
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
		recv_request(q,connfd);
		handle_request(q);
		close(connfd);
	}
}
