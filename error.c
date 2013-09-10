#include <string.h>
#include "io.h"
#include "request.h"
#include "error.h"

void handle_error(request_t *q, char *name, int errno)
{
	char buf[MAXLINE], body[MAXBUF],errnum[MAXLINE],msg[MAXLINE],title[MAXLINE];

	sprintf(body, "<html><title>XMJ error</title>\r\n");
	switch(errno)
	{
	case NO501:
		strcpy(errnum,"501");
		strcpy(msg,"We doesnt implement the method");
		strcpy(title,"Not Implemented");
		break;
	case NO403:
		strcpy(errnum,"403");
		strcpy(msg,"The read/run operation is denied");
		strcpy(title,"Forbidden");
		break;
	case NO404:
		strcpy(errnum,"404");
		strcpy(msg,"Counldnt find the resource file");
		strcpy(title,"Not found");
		break;
	}

	sprintf(body,"%s%s: %s\r\n",body,errnum,title);
	sprintf(body,"%s<p>%s : %s</p>\r\n",body, msg, name);

	sprintf(buf, "%s %s %s\r\n",q->ver,errnum,title);
	io_writen(q->io, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	io_writen(q->io, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n",(int)strlen(body));
	io_writen(q->io, buf, strlen(buf));
	io_writen(q->io, body, strlen(body));
}
