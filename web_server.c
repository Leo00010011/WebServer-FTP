#include <stdio.h>
#include "server_tools.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "http_tools.h"
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <wait.h>

linked_list *chld;
int listen_fd;

void SIGINT_handler()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    linked_node *current = chld->first;
    while (current != NULL)
    {
        kill(*((int *)current->value), SIGKILL);
        waitpid(*((int *)current->value), NULL, WNOHANG);
        free(current->value);
        remove_node(chld,current);
        current = current->next;
    }
    close(listen_fd);
    exit(0);
}

void SIGCHLD_handler()
{
    int pid;
    linked_node *current;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        current = chld->first;
        while (current != NULL)
        {
            if (*((int *)current->value) == pid)
            {
                free(current->value);
                remove_node(chld, current);
                break;
            }
            current = current->next;
        }
    }
}

void process(int connect_fd, int listen_fd, char *default_url)
{
    http_request *request = read_request(connect_fd);
    if (request == NULL)
    {
        printf("invalid request\n");
        write_error_message("400", "Bad request", connect_fd);
        return;
    }
    if (strcmp(request->url, "/") == 0)
    {
        free(request->url);
        request->url = malloc(strlen(default_url) + 1);
        int pos = my_strcopy(request->url,default_url,0);
        request->url[pos] = '\0';
    }
    int pid;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    if ((pid = fork()) == 0)
    {
        close(listen_fd);
        char *url_ptr = request->url;
        /*mover el puntero hasta encontrar uno de los  dos símbolos*/
        while (*url_ptr != '\0' && *url_ptr != '?')
        {
            url_ptr++;
        }
        /*si es descarga*/
        if (*url_ptr == '\0')
        {
            download_file(request->url, url_ptr, connect_fd);
            free_http_request(request);
            exit(0);
        }
        *url_ptr = '\0';
        char *argv[3];
        argv[0] = request->url + 1;
        argv[1] = url_ptr + 1;
        argv[2] = NULL;
        dup2(connect_fd, STDOUT_FILENO);

        if (execv(argv[0], argv) < 0)
        {
            write_error_message("404", "Not found", connect_fd);
            free_http_request(request);
            exit(1);
        }
    }
    int *value = malloc(sizeof(int));
    *value = pid;
    add_last(chld, value);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    free_http_request(request);
}


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <Port> <Directory>", argv[0]);
        return 1;
    }
    char *endptr;

    int port = strtol(argv[1], &endptr, 0);
    if (*endptr != '\0')
    {
        fprintf(stderr, "%s: Invalid port %s", argv[0], argv[1]);
        return 1;
    }
    opendir(argv[2]);
    if (errno == ENOENT)
    {
        fprintf(stderr, "%s: Invalid directory %s", argv[0], argv[2]);
        return 1;
    }
    if ((listen_fd = init_listen_socket(port)) < 0)
    {
        return 1;
    }
    char *default_url = malloc(strlen(argv[2]) + 13);
    sprintf(default_url, "/sortbyname?%s", argv[2]);
    chld = create_linked_list();
    int connect_fd;
    struct sockaddr_in client_socket;
    int client_len;
    signal(SIGINT, SIGINT_handler);
    signal(SIGCHLD, SIGCHLD_handler);
    while (1)
    {

        client_len = sizeof(struct sockaddr_in);
        if ((connect_fd = accept(listen_fd, &client_socket, &client_len)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                exit(1);
            }
        }
        printf("connected: %s\n", inet_ntoa(client_socket.sin_addr));
        process(connect_fd, listen_fd, default_url);
        close(connect_fd);
        printf("disocnected\n");
    }
}
