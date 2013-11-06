#include "request.h"
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define 		MAX_FD			100

int setnonblocking(int sock)
{
	    int opts;
	    opts=fcntl(sock,F_GETFL);
	    if(opts<0)
        {
		   fprintf(stderr,"fcntl(sock,GETFL)");
		   return -1;
		}
		opts = opts|O_NONBLOCK;
		if(fcntl(sock,F_SETFL,opts)<0)
		{
			fprintf(stderr,"fcntl(sock,SETFL,opts)");
		     return -1;
		}
		return 0;
}

int main(int argc, char **argv)
{
	int listenfd, epollfd, connfd, port, i;
	socklen_t  clientlen;
	int nready;
	struct sockaddr_in clientaddr;
	struct epoll_event ev, events[MAX_FD];

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

	epollfd = epoll_create(MAX_FD);
	if (epollfd == -1) {
	    fprintf(stderr,"epoll_create error\n");
	    exit(-1);
	}
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
	    fprintf(stderr, "epoll_ctl: listen_sock\n");
	    exit(-1);
	}

	for(;;)
	{
		clientlen = sizeof(clientaddr);
		nready = epoll_wait(epollfd, events, MAX_FD, -1);//get ready fd
		if (nready == -1) {
	        fprintf(stderr,"epoll_wait error !\n");
	        exit(-1);
	    }
		for (i = 0; i < nready; ++i) {
			if (events[i].data.fd == listenfd) {
	        connfd = accept(listenfd,
            (struct sockaddr *) &clientaddr, &clientlen);
	        if (connfd == -1) {
	 	       fprintf(stderr,"accept error !\n");
               exit(-1);
            }
            setnonblocking(connfd);
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = connfd;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                fprintf(stderr,"epoll_ctl: conn_sock error !\n");
	            exit(-1);
		    }
			}
			else{
			recv_request(q,events[i].data.fd);
			handle_request(q);
			close(connfd);
			}
		}
	}

	return 0;
}
