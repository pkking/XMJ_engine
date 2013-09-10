#ifndef _IO_H_
#define _IO_H_
#include <stdio.h>
#include <unistd.h>

//data structure
#define IO_BUFSIZE		8192
typedef struct
{
	int io_fd;
	int io_cnt;
	char *io_bufptr;
	char io_buf[IO_BUFSIZE];
}io_t;
// return byte numbers if successful, -1 when failed
extern ssize_t io_readn(io_t *rp, void *buf, size_t n);//read n bytes from filedes
extern ssize_t io_writen(io_t *rp, const void *buf, size_t n);//write n bytes into filedes
extern ssize_t io_readline(io_t *rp, void *buf, size_t len);//read line from filedese
extern void io_readinit(io_t *rp, int fd);

#endif
