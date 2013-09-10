#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <stdint.h>
#include "io.h"

#define MAXLINE 	8192   
#define GET			"GET"
#define HEAD 		"HEAD"
enum{READ, RUN};

typedef struct __request_t request_t;
struct __request_t
{
	int fd;
	io_t *io;
	char resource[MAXLINE];
	char method[MAXLINE];
	char uri[MAXLINE];
	char ver[MAXLINE];
	char host;
	int need_cgi;
	//char *msg_box;
	char cgi_args[MAXLINE];
	char header[MAXLINE];
};

//extern XMJ_open_clientfd(char *hostname, int port);
//extern int wait_for_request(request_t *req, int port);
extern int open_listenfd(int port);
extern void recv_request(request_t *q, int fd);
extern void handle_request(request_t *q);
extern void read_request_header(request_t *q);
extern int parse_uri(char *uri, char *filename, char *cgi_args);

#endif
