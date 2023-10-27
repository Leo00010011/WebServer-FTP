#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stddef.h>
#include "server_tools.h"
#include <string.h>

int connect_by_hostname(char *host_name, int port)
{
    struct hostent *host;
    struct sockaddr_in addr;

    if ((host = gethostbyname(host_name)) == NULL)
    {
        return -2;
    }
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
    addr.sin_port = htons(port);

    return connect_by_sockaddr(&addr);
    
}

int connect_by_sockaddr(struct sockaddr_in *addr)
{
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }

    if (connect(fd, addr, sizeof(struct sockaddr_in)) < 0)
    {
        return -1;
    }
    return fd;
}

int connect_by_addr(char* addr, int port)
{
    struct in_addr sin_addr;
    if(inet_aton(addr,&sin_addr) < 0)
    {
        return -1;
    }

    struct sockaddr_in sock_addr;
    memset(&sock_addr,0,sizeof(struct sockaddr_in));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr = sin_addr;
    sock_addr.sin_port = htons(port);

    return connect_by_sockaddr(&sock_addr);
}

int init_listen_socket(int port)
{
    int listenfd, optval = 1;
    struct sockaddr_in saddr_in;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
    {
        return -1;
    }
    
    memset(&saddr_in, 0, sizeof(struct sockaddr_in));
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr_in.sin_port = htons((unsigned short)port);
    int s = sizeof(saddr_in);

    if (bind(listenfd, (struct sockaddr *)&saddr_in, s) < 0)
    {
        return -1;
    }

    if (listen(listenfd, 1024) < 0)
    {
        return -1;
    }
    return listenfd;
}
