#ifndef _ERROR_H_
#define _ERROR_H_

#include "request.h"

#define MAXBUF	8192 
enum{NO200,NO501,NO404,NO403};

void handle_error(request_t *q, char *name, int errno);

#endif
