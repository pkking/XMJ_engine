#include <string.h>
#include "cgi.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <wait.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
uint32_t cgi_parser(cgi_form *cgi, char *input, uint32_t length, char *output)
{
	int i,val_count=0;

	if(cgi == NULL)
		return CGI_NOT_INIT;
	
	while(strsep(&input, '&'))
	{
		val_count++;
	}
}0*/
static void get_filetype(char *filename, char *filetype);

void put_static_info(request_t *q, char *filename, int filesize)
{
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXLINE];

	get_filetype(filename, filetype);
	sprintf(buf, "%s 200 OK\r\n",q->ver);
	sprintf(buf, "%sContent-length:%d\r\n",buf,filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	io_writen(q->io, buf, strlen(buf));

	srcfd = open(filename, O_RDONLY, 0);
	srcp = mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	close(srcfd);
	io_writen(q->io, srcp, filesize);
	munmap(srcp, filesize);
}

static void get_filetype(char *filename, char *filetype)
{
	if(strstr(filename, ".html"))
		strcpy(filetype,"text/html");
	else if(strstr(filename, ".gif"))
		strcpy(filename,"image/gif");
	else if(strstr(filename, ".jpg"))
		strcpy(filename, "image/jpeg");
	else
		strcpy(filename, "text/plain");
}

void run_cgi(request_t *q, char *filename, char *cgi_args)
{
	char buf[MAXLINE], *emptylist[]={NULL};

	sprintf(buf, "%s 200 OK\r\n",q->ver);
	io_writen(q->io, buf, strlen(buf));
	
	if(fork() == 0)
	{
		setenv("QUERY_STRING", cgi_args, 1);
		dup2(q->io->io_fd, STDOUT_FILENO);
		execve(filename, emptylist, environ);
	}
	//else
	//	wait(NULL);
}
