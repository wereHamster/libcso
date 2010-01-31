
#ifndef __CSO_H__
#define __CSO_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct cso
{
    int nSocket, socket[0];
} cso_t;

cso_t *csocr(const char *node, const char *service, struct addrinfo *hints, int backlog);
int csoea(cso_t *cso, int (*cb)(int fd, void *ud), void *ud);
void csorm(cso_t *cso);

#endif /* __CSO_H__ */
