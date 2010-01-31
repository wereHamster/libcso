
#include <cso.h>

#include <stdlib.h>
#include <unistd.h>

cso_t *csocr(const char *node, const char *service, struct addrinfo *hints, int backlog)
{
    struct addrinfo *res;
    int err = getaddrinfo(node, service, hints, &res);
    if (err)
	return NULL;
    
    cso_t *ret = calloc(sizeof(cso_t) + 10 * sizeof(int), 1);
    for (struct addrinfo *ai = res; ai != NULL; ai = ai->ai_next) {
	int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sockfd < 0)
	    goto fail;
	
	int err = bind(sockfd, ai->ai_addr, ai->ai_addrlen);
	if (err) {
	    close(sockfd);
	    goto fail;
	}

	err = listen(sockfd, backlog);
	if (err) {
	    close(sockfd);
	    goto fail;
	}

	ret->socket[ret->nSocket++] = sockfd;
    }

    freeaddrinfo(res);
    return ret;
    
fail:
    freeaddrinfo(res);

    for (; ret->nSocket > 0; --ret->nSocket) {
	close(ret->socket[ret->nSocket - 1]);
    }
    
    free(ret);
    return NULL;
}

int csoea(cso_t *cso, int (*cb)(int fd, void *ud), void *ud)
{
    for (int i = 0; i < cso->nSocket; ++i) {
	int err = (*cb)(cso->socket[i], ud);
	if (err)
	    return err;
    }

    return 0;
}

void csorm(cso_t *cso)
{
    for (; cso->nSocket > 0; --cso->nSocket) {
	close(cso->socket[cso->nSocket - 1]);
    }

    free(cso);
}
