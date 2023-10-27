#if !defined(SERVER_TOOLS)
#define SERVER_TOOLS
#include <netinet/in.h>

int connect_by_hostname(char *host_name, int port);

int connect_by_sockaddr(struct sockaddr_in *addr);

int connect_by_addr(char* addr, int port);

/*inicializa un listen socket listo para aceptar conexiones
retorna el fd, si hay algun error retorna -1*/
int init_listen_socket(int port);

#endif // SERVER_CLIENT_TOOLS
