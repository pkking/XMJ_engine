#include "request.h"
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//信号处理
/*
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
*/
int main(int argc, char **argv)
{
//	signal(SIGCHLD, sig_chld);
	int listenfd, connfd, port, client[FD_SETSIZE],maxi,i,max_fd;
	socklen_t  clientlen;
	fd_set read_set;
	int nready;
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

	FD_ZERO(&read_set);
	FD_SET(listenfd, &read_set);
	max_fd=listenfd;
	maxi=0;

	for(i=0;i<FD_SETSIZE;i++)
	{
		client[i] = -1;
	}
	while(1)
	{
		clientlen = sizeof(clientaddr);
		nready = select(max_fd+1,&read_set,NULL,NULL,NULL);

		if(FD_ISSET(listenfd,&read_set))
		{
			connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
			if(maxi>FD_SETSIZE)
			{
				printf("too many client!\n");
				exit(-1);
			}
			for(i=0;i<FD_SETSIZE;i++)
			{
				if(client[i]<0)
				{
					client[i]=connfd;
					break;
				}
			}

			FD_SET(connfd,&read_set);
			if(connfd>max_fd)
				max_fd=connfd;
			if(i>maxi)
				maxi=i;
			if(--nready<=0)//if just the listen socket get ready
				continue;
		}
			//recv_request(q,connfd);	
			//handle_request(q);
			//close(connfd)
		
		for(i=0;i<FD_SETSIZE;i++)
		{
			if(client[i]==-1)
				continue;
			if(FD_ISSET(client[i],&read_set))
			{
				recv_request(q,client[i]);
				handle_request(q);
				close(connfd);
				FD_CLR(client[i],&read_set);
				client[i]=-1;
			}
			if(--nready<=0)
				break;
		}
	}
}
