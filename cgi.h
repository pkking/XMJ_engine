#ifndef _CGI_H_
#define _CGI_H_

/*  what the CGI scrpit do:
 *
 *	1. read the user's form input.
 *	2. Do what you want with the data.
 *	3. Write the HTML response to STDOUT.
 *
 */
#include <stdint.h>
#include "request.h"

typedef struct __cgi_form cgi_from;
struct __cgi_form
{
	uint32_t val_num;
	char *name;
	char *value;
	char *path_name;
};


extern char **environ;
extern void run_cgi(request_t *q, char *filename, char *cgi_args);
extern void put_static_info(request_t *q, char *filename, int filesize);

#endif
