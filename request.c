#include "request.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "io.h"
#include <string.h>
#include <sys/stat.h>
#include "error.h"
#include <stdlib.h>
#include "cgi.h"


static int is_support_method(char *method);
static int has_the_resource(char *filename, struct stat *mode);
static int has_the_right(char *filename, struct stat *mode, int right);

int open_listenfd(int port)
{
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0) //it`s a TCP link
	{
		close(listenfd);
		return -1;
	}

	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
				(const void*)&optval, sizeof(int))<0)
	{
		close(listenfd);
		return -1;
	}

	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if(bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))<0)
	{
		close(listenfd);
		return -1;
	}

	if(listen(listenfd, 1024)<0)
	{
		close(listenfd);
		return -1;
	}
	return listenfd;
}

void recv_request(request_t *q, int fd)
{
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], ver[MAXLINE];
	q->io=(io_t*)malloc(sizeof(io_t));

	io_readinit(q->io,fd);
	io_readline(q->io, buf, MAXLINE);//read HTTP request like "GET / HTTP/1.1"
	sscanf(buf, "%s %s %s",method, uri, ver);
	strcpy(q->method, method);
	strcpy(q->ver, ver);
	strcpy(q->uri, uri);
}

void handle_request(request_t *q)
{
	int need_cgi;
	struct stat mode;
	char filename[MAXLINE], cgi_args[MAXLINE];

	if(is_support_method(q->method))
	{
		handle_error(q, q->method , NO501);
		return ;
	}
	read_request_header(q);

	need_cgi = parse_uri(q->uri, filename, cgi_args); // check if it will run a cgi scrpit and get the cgi args
	q->need_cgi = need_cgi;

	 //init struct stat
	if(stat(filename, &mode) == -1 || has_the_resource(filename, &mode)) //check if there is the resource
	{
		handle_error(q, filename, NO404);
		return;
	}

	if(need_cgi)
	{
		if(has_the_right(filename, &mode, RUN))
		{
			run_cgi(q, filename, cgi_args);
			strcpy(q->cgi_args, cgi_args);
		}
	}
	else
	{
		if(has_the_right(filename, &mode, READ))
			put_static_info(q, filename, mode.st_size);
	}
}

void read_request_header(request_t *q)
{
	char buf[MAXLINE];

	io_readline(q->io, buf, MAXLINE);
	while(strcmp(buf, "\r\n"))
	{
		io_readline(q->io, buf, MAXLINE);
		strcpy(q->header, buf);
	}
	return ;
}

//return 0 if it neednt cgi
int parse_uri(char *uri, char *filename, char *cgi_args)
{
	char *ptr;

	if(!strstr(uri, "cgi-bin"))
	{
		strcpy(cgi_args, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if(uri[strlen(uri)-1] == '/')
			strcat(filename, "index.html");//default ref to "/index.html"
		return 0;
	}
	else
	{
		ptr = strchr(uri, '?');
		if(ptr)
		{
			strcpy(cgi_args, ptr+1);
			*ptr = '\0';
		}
		else
			strcpy(cgi_args,"");//no args
		strcpy(filename, ".");
		strcat(filename, uri);
		return 1;
	}
}

static int is_support_method(char *method)
{
	if(!strcasecmp(method, GET) || !strcasecmp(method, HEAD))
	{
		return 0;
	}
	else
		return -1;
}

static int has_the_resource(char *filename, struct stat *mode)
{
	if(S_ISREG(mode->st_mode))
		return 0;
	else
		return -1;
}

static int  has_the_right(char *filename, struct stat *mode, int right)
{
	switch(right)
	{
		case READ:
			if(!(S_IRUSR & mode->st_mode))
				return -1;
			break;
		case RUN:
			if(!(S_IXUSR & mode->st_mode))
				return -1;
			break;
	}

	return 0;
}
