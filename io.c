#include "io.h"
#include <errno.h>
#include <string.h>

void io_readinit(io_t *rp, int fd)
{
	rp->io_fd = fd;
	rp->io_cnt = 0;
	rp->io_bufptr = rp->io_buf;
}

static ssize_t io_read(io_t *rp, void *buf, size_t n)
{
	int cnt;

	while(rp->io_cnt <= 0)
	{
		rp->io_cnt = read(rp->io_fd, rp->io_buf, sizeof(rp->io_buf));
		if(rp->io_cnt < 0)
		{
			if(errno != EINTR)
				return -1;
		}
		else if(rp->io_cnt == 0)
			return 0;
		else
			rp->io_bufptr = rp->io_buf;
	}

	cnt = n;
	if(rp->io_cnt < n)
		cnt = rp->io_cnt;
	memcpy(buf, rp->io_bufptr, cnt);
	rp->io_bufptr += cnt;
	rp->io_cnt -= cnt;
	return cnt;
}

ssize_t io_readn(io_t *rp, void *buf, size_t n)
{
	ssize_t nleft = n;
	ssize_t nread;
	char *ptr = buf;

	while(nleft > 0)
	{
		if((nread = io_read(rp, ptr, nleft))<0)
		{
			if(errno == EINTR)
				nread = 0;   //the reading was interrupted, just go on reading
			else
				return(-1);
		}
		else if(nread == 0)//read EOF
		{
			break;
		}
		nleft -= nread; 
		ptr += nread; //mv ptr in buff for next reading
	}
	return(n-nleft); //actully read number of bytes
}

ssize_t io_writen(io_t *rp, const void *buf, size_t n)
{
	ssize_t nleft;
	ssize_t nwriten;
	char *ptr;
	int fd = rp->io_fd;

	ptr = (char *)buf;
	nleft = n;
	while(nleft>0)
	{
		if((nwriten = write(fd, ptr, nleft))<=0)
		{
			if(errno == EINTR)
			{
				nwriten = 0;
			}
			else
				return(-1);
		}
		
		nleft -= nwriten;
		ptr += nwriten;

	}
	return (n);
}

//slow in reading
ssize_t io_readline(io_t *rp, void *buf, size_t len)
{
	ssize_t rc,n;
	char c,*ptr=buf;

	for(n=1;n<len;n++)
	{
		if((rc = io_read(rp, &c, 1)) ==1)
		{
			*ptr++ =c;
			if(c == '\n')
				break; //new line
		}
		else if(rc == 0)
		{
			if(n == 1)
				return 0; //no data read
			else
				break; // something were read
		}
		else //case rc < 0
		{
			return -1;
		}
	}

	*ptr=0;
	return n;
}
