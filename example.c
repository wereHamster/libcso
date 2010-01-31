
#include <cso.h>
#include <errno.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

struct setupdata
{
    fd_set readset;
    int maxfd;
};

static int setup(int fd, void *ud)
{
    struct setupdata *data = ud;

    printf("setup fd %d\n", fd);
    FD_SET(fd, &data->readset);
    if (fd > data->maxfd)
	data->maxfd = fd;

    return 0;
}

static int handle(int fd, void *ud)
{
    struct setupdata *data = ud;

    if (!FD_ISSET(fd, &data->readset))
	return 0;

    int clientfd = accept(fd, NULL, NULL);
    if (clientfd >= 0) {
	printf("accepted connection\n");
	close(clientfd);
    } else {
	perror("accept");
    }

    return clientfd >= 0;
}

int main(int argc, char *argv[])
{
    struct addrinfo hints = {
	.ai_family = PF_UNSPEC,
	.ai_socktype = SOCK_STREAM,
	.ai_flags = AI_PASSIVE
    };

    cso_t *cso = csocr("localhost", "8080", &hints, 8);
    if (cso == NULL)
	return -1;

    struct setupdata data;
 
    int result;
    do {
	FD_ZERO(&data.readset);
	data.maxfd = -1;
	csoea(cso, setup, &data);
	printf("maxfd: %d\n", data.maxfd);
	result = select(data.maxfd + 1, &data.readset, NULL, NULL, NULL);
    } while (result == -1 && errno == EINTR);

    printf("select returned\n");
    if (result > 0) {
	csoea(cso, handle, &data);
    } else if (result < 0) {
	perror("select");
    }

    return 0;
}
