#include "http_tools.h"
#include "structures.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "fd_tools.h"
#include <errno.h>
#include <unistd.h>
#include "http_tools.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <sys/sendfile.h>


int my_strcopy(char *dest, char *source, int start_pos)
{
    int pos = 0;
    while (source[pos] != '\0')
    {
        dest[start_pos + pos] = source[pos];
        pos++;
    }
    return start_pos + pos;
}

void write_error_message(char *code, char *text, int fd)
{
    char *result = malloc(strlen(code) + strlen(text) + 17);
    sprintf(result, "HTTP/1.1 %s %s\r\n\r\n\r\n", code, text);
    write_rio(fd, result, strlen(result));
    free(result);
}

int fill_response_len(http_response *response)
{
    response->len = 0;
    response->len += strlen(response->status_c);
    response->len += strlen(response->status_m);
    response->len += strlen(response->version);
    response->len += 4; //los dos espacios y el \r\n
    if (response->headers != NULL)
    {
        linked_node *current = response->headers->first;
        while (current != NULL)
        {
            response->len += strlen(((pair_key_value *)current->value)->key);
            response->len += strlen(((pair_key_value *)current->value)->value);
            response->len += 4; // el tamanno del \r\n y del ": "
            current = current->next;
        }
    }
    response->len += 2; //el \r\n que viene despues de los headers
    if (response->content != NULL)
    {
        response->len += strlen(response->content);
        response->len += 2; //el \r\n que viene despues del content
    }
    return response->len;
}

int write_response_small_content(http_response *response, int socket_fd)
{
    char *result = calloc(response->len, 1);
    int pos = 0;
    int i = my_strcopy(result, response->version, 0);
    result[i] = ' ';
    i++;
    i = my_strcopy(result, response->status_c, i);
    result[i] = ' ';
    i++;
    i = my_strcopy(result, response->status_m, i);
    i = my_strcopy(result, "\r\n", i);
    if (response->headers != NULL)
    {
        linked_node *current = response->headers->first;
        while (current != NULL)
        {
            i = my_strcopy(result, ((pair_key_value *)current->value)->key, i);
            i = my_strcopy(result, ": ", i);
            i = my_strcopy(result, ((pair_key_value *)current->value)->value, i);
            i = my_strcopy(result, "\r\n", i);
            current = current->next;
        }
    }

    i = my_strcopy(result, "\r\n", i);
    if (response->content != NULL)
    {
        i = my_strcopy(result, response->content, i);
        my_strcopy(result, "\r\n", i);
    }
    int len = write_rio(socket_fd, result, response->len);
    free(result);
    return len;
}

void free_headers_list(linked_list *list)
{
    linked_node *current = list->first;
    while (current != NULL)
    {
        free(((pair_key_value *)current->value)->value);
        free(((pair_key_value *)current->value)->key);
        free(current->value);
        if (current->next == NULL)
        {
            free(current);
            break;
        }
        current = current->next;
        free(current->preview);
    }
    free(list);
}

int read_rio_http(int fd, char *buffer, int size, bool *end)
{
    int left = size;
    int readed;
    char *bufp = buffer;
    *end = false;
    while (left > 0)
    {
        if ((readed = read(fd, bufp, left)) < 0)
        {
            if (errno == EINTR)
            {
                readed = 0;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            left -= readed;
            if (readed == 0 || (size - left >= 4 && buffer[size - left - 4] == '\r' && buffer[size - left - 3] == '\n' &&
                                buffer[size - left - 2] == '\r' && buffer[size - left - 1] == '\n'))
            {
                if (end = NULL)
                {
                    *end = true;
                }
                break;
            }
            left += readed;
        }
        left -= readed;
        bufp += readed;
    }
    return size - left;
    
}

int refresh_file_http(my_file_http *file, int size)
{
    int readed;
    file->buff = malloc(size);
    if ((readed = read_rio_http(file->fd, file->buff, size, &file->end)) < 0)
    {
        return -1;
    }
    file->buff_size = readed;
    file->buff_pos = 0;
    return 0;
}

char *gettchar_http(my_file_http *file, char c)
{

    if (file->buff_size == 0)
    {
        return NULL;
    }
    char *result = NULL;
    int last_size = 0;
    int i;
    bool founded = false;
    while (1)
    {
        i = 0;
        //buscar el char
        for (; file->buff_pos + i < file->buff_size; i++)
        {
            if (file->buff[file->buff_pos + i] == c)
            {
                founded = true;
                break;
            }
        }
        //reservando espacio para guardar lo que se encontro, uno mas para poner el '\0'
        result = realloc(result, last_size + i + 1);
        //copiando lo que se encontro
        for (int j = 0; j < i; j++)
        {
            result[last_size + j] = file->buff[file->buff_pos + j];
        }
        //si se llego a size no se encontro el caracter
        if (!founded)
        {
            if (file->end) //se llego al final del request
            {
                free(result);
                return NULL;
            }
            //refrescando el buffer
            free(file->buff);
            refresh_file_http(file, file->buff_size);
            last_size += i; //no se tiene en cuenta el espacio extra para que el tamanno no vaya incrementando por cada iteracion
            continue;
        }
        else //si no se llego al final del buffer fue que se termino
        {
            result[last_size + i] = '\0'; //guarda 0 en el espacio reservado
            file->buff_pos += i;          // se pone el file en una posicion correcta(apuntando al caracter que no tomo)
            return result;
        }
    }
}

int inc_pos_sock_http_request(my_file_http *file, int inc, char **buff_guard)
{
    //comprobando si la estructura esta en un estado correcto
    if (file->buff_size == 0)
    {
        return -1;
    }
    //si no hay que leer solo se modifica el pos
    int new_pos = file->buff_pos + inc;
    if (new_pos < file->buff_size)
    {
        file->buff_pos = new_pos;
        return 1;
    }
    // si hay que llamar read entonces se piden suficientes bytes para que el nuevo pos apunte al principio del buffer

    char *temp_buff = malloc(new_pos);
    int readed;

    if ((readed = read_rio_http(file->fd, temp_buff, new_pos, &file->end)) < ((new_pos - file->buff_size) + 1))
    {
        if (readed < 0)
        {
            free(temp_buff);
            return -1;
        }
        if (buff_guard == NULL)
        {
            free(temp_buff);
        }
        else
        {
            *buff_guard = temp_buff;
        }
        return 0;
    }

    file->buff_pos = 0;
    if (readed < new_pos)
    {
        file->buff_size -= (new_pos - readed);
        free(file->buff);
        file->buff = malloc(file->buff_size);
    }
    for (int i = 0; i < file->buff_size; i++)
    {
        file->buff[file->buff_size - 1 - i] = temp_buff[readed - 1 - i];
    }
    if (buff_guard == NULL)
    {
        free(temp_buff);
    }
    else
    {
        *buff_guard = temp_buff;
    }
    return 1;
}

int get_hex_value(char ch)
{
    if ('0' <= ch && '9' >= ch)
    {
        return ch - '0';
    }
    else
    {
        if ('A' <= ch && ch <= 'F')
        {
            return ch - 'A' + 10;
        }
        else
        {
            return -1;
        }
    }
}

char *parse_from_percent(char *s)
{
    int count = 0;
    int i = 0;
    for (; s[i] != '\0'; i++)
    {
        if (s[i] == '%')
        {
            count++;
        }
    }

    char *result = malloc(i + 1 - 2 * count);

    i = 0;
    int j = 0;
    for (; s[i] != '\0'; j++)
    {
        if (s[i] == '%')
        {
            result[j] = 16 * get_hex_value(s[i + 1]) + get_hex_value(s[i + 2]);
            i += 3;
        }
        else
        {
            result[j] = s[i];
            i++;
        }
    }
    result[j] = '\0';
    return result;
}

void free_http_request(http_request *request)
{
    free(request->method);
    free(request->url);
    free(request->version);
    free_headers_list(request->headers);
}

void print_headers(linked_list *list)
{
    linked_node *current = list->first;
    while (current != NULL)
    {
        printf("%s: %s\n", ((pair_key_value *)current->value)->key, ((pair_key_value *)current->value)->value);
        current = current->next;
    }
}

http_request *read_request(int socket_fd)
{
    http_request *request = malloc(sizeof(http_request));
    my_file_http file;
    file.fd = socket_fd;
    char *current_item;
    if (refresh_file_http(&file, 150) < 0 || file.buff_size == 0)
    {
        return NULL;
    }
    //obteniendo method
    if ((current_item = gettchar_http(&file, ' ')) == NULL)
    {
        free(file.buff);
        free(request);
        return NULL;
    }
    else
    {
        request->method = current_item;
    }
    if (inc_pos_sock_http_request(&file, 1, NULL) != 1)
    {
        free(file.buff);
        free(request->method);
        free(request);
        return NULL;
    }

    //obteniendo la url
    if ((current_item = gettchar_http(&file, ' ')) == NULL)
    {
        free(file.buff);
        free(request->method);
        free(request);
        return NULL;
    }
    else
    {
        request->url = parse_from_percent(current_item);
        free(current_item);
    }

    if (inc_pos_sock_http_request(&file, 1, NULL) != 1)
    {
        free(file.buff);
        free(request->url);
        free(request->method);
        free(request);
        return NULL;
    }

    //obteniendo la version
    if ((current_item = gettchar_http(&file, '\r')) == NULL)
    {
        free(file.buff);
        free(request->method);
        free(request->url);
        free(request);
        return NULL;
    }
    else
    {
        request->version = current_item;
    }

    //obteniendo los headers
    int result;
    request->headers = create_linked_list();
    pair_key_value *pair;
    while ((result = inc_pos_sock_http_request(&file, 2, NULL)) == 1 && file.buff[file.buff_pos] != '\r')
    {
        pair = malloc(sizeof(pair_key_value));

        if ((current_item = gettchar_http(&file, ':')) == NULL)
        {
            free(pair);
            result = -1;
            break;
        }

        pair->key = current_item;
        if (inc_pos_sock_http_request(&file, 2, NULL) != 1)
        {
            free(pair->key);
            free(pair);
            result = -1;
            break;
        }

        if ((current_item = gettchar_http(&file, '\r')) == NULL)
        {
            free(pair->key);
            free(pair);
            free(current_item);
            result = -1;
            break;
        }

        pair->value = current_item;
        add_last(request->headers, pair);
    }

    free(file.buff);
    if (result == -1)
    {
        free(request->method);
        free(request->url);
        free(request->version);
        free_headers_list(request->headers);
        free(request);
        return NULL;
    }
    return request;
}

int write_response_file_content(http_response *response, char *file_path, int socket_fd)
{
    int fd = open(file_path, O_RDONLY, 0); 
    struct stat st;
    fstat(fd, &st);
    pair_key_value *pair = malloc(sizeof(pair_key_value));
    pair->key = "Content-Length";
    pair->value = malloc(floor(log10(st.st_size)) + 1);
    sprintf(pair->value, "%d", st.st_size);
    response->len += 14 + 6 + strlen(pair->value);
    add_last(response->headers, pair);
    response->content = NULL;
    write_response_small_content(response, socket_fd);

    //manejar el shortcount del send
    int left = st.st_size;
    int transferred;
    long offset = 0;
    while (left > 0)
    {
        //error transeferring
        if ((transferred = sendfile(socket_fd, fd, &offset, left)) < 0)
        {
            if (errno == EINTR)
            {
                transferred = 0;
            }
            else
            {
                perror("error transferring");
                return -1;
            }
        }
        left -= transferred;
    }

    write_rio(socket_fd, "\r\n", 2);
}

void download_file(char *url, char *url_ptr, int connect_fd)
{

    struct stat st;

    if (stat(url, &st) < 0 || !S_ISREG(st.st_mode) || access(url, R_OK) < 0)
    {
        printf("Not found\n");
        printf("url: %s\n", url);
        write_error_message("404", "Not found", connect_fd);
        exit(1);
    }
    if (access(url, R_OK) < 0)
    {
        printf("Forbidden\n");
        printf("url: %s\n", url);
        write_error_message("403", "Forbidden", connect_fd);
        exit(1);
    }
    http_response response;
    response.status_c = "200";
    response.status_m = "OK";
    response.version = "HTTP/1.1";
    response.headers = create_linked_list();
    pair_key_value *pair = malloc(sizeof(pair_key_value));
    pair->key = "Content-Type";
    pair->value = "application/octet-stream";
    add_last(response.headers, pair);
    pair = malloc(sizeof(pair_key_value));
    pair->key = "Content-Disposition";
    int count = 0;
    while (*(url_ptr - 1) != '/')
    {
        url_ptr--;
        count++;
    }
    pair->value = malloc(count + 25);
    int pos = my_strcopy(pair->value, "attachment; filename =\"", 0);
    pos = my_strcopy(pair->value, url_ptr, pos);
    my_strcopy(pair->value, "\"", pos);
    ((char *)pair->value)[count + 24] = '\0';
    add_last(response.headers, pair);
    response.len = 104 + count;
    write_response_file_content(&response, url, connect_fd);
    linked_node* current = response.headers->first;
    free(pair->value);
    free(((pair_key_value*)response.headers->last->value)->value);
}