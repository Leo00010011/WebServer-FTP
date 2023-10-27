#if !defined(HTTP_TOOLS_GUARD)
#define HTTP_TOOLS_GUARD

#include "structures.h"
#include "fd_tools.h"
#include <stdbool.h>
typedef struct http_request
{
    char *method;
    char *url;
    char *version;
    linked_list *headers;
} http_request;

typedef struct http_response
{
    char *version;
    char *status_c;
    char *status_m;
    linked_list *headers;
    char *content;
    int len;
} http_response;

typedef struct my_file_http
{
    int fd;
    char *buff;
    int buff_size;
    int buff_pos;
    bool end;
}my_file_http;

/*copia source, en dest a partir de start_pos
retorna indice sigiuente al ultimo que copio*/
int my_strcopy(char *dest, char *source, int start_pos);

/*recibe un fd resultado de un accept y parsea la peticion http*/
http_request *read_request(int socket_fd);

/*escribe en un socket resultado de un accept, una respuesta http con el contenido
del struct http_response 
si el content es NULL no escribe ni el \\r\\n, con la intencion de que sea el usuario el que lo haga*/
int write_response_small_content(http_response *response, int socket_fd);

/*escribe una respuesta con el response line y los headers indicados en response
El metodo es responsable de agregar el header de content length,agregar su tamanno al response len,transferir el archivo correctamente y poner el \\r\\n del final*/
int write_response_file_content(http_response *response,char* file_path, int socket_fd);

/*usa el mismo codigo que read_rio para manejar los shortcount y los signals, la unica diferencia es que 
para verificar el fin del contenido , en vez de volver a llamar read para recibir eof, el comprueba que 
los cuatro ultimos caracteres sean "\\r\\n\\r\\n", para evitar el caso en que servidor y cliente llemen
read sus respectivos sockets a la vez
Si end != NULL entonces se guarda si con esta lectura se termino de leer el archivo*/
int read_rio_http(int fd, char *buffer, int size, bool* end);

/*calcula el valor del len del request y se lo asigna
retorna el valor del len
si content es null no calcula ni el \\r\\n con intencion de que el usuario lo haga*/
int fill_response_len(http_response *response);

/*incrementa pos correctamente, retorna -1 si pos cae despues del eof
no usa lseek para mover el pos del fd por si el fd es un socket
retorna -1 si ocurrio un error, 0 si el pos cae fuera del archivo, 1 si se pudo actualizar correctamente
en caso de no poderse actualizar correctamente se deja el file en el estado original pero el pos del fd si es modificado
para estos casos se permite pasar un puntero al que se le asignara el valor de la posicion de un espacio de memoria donde
estara guardado toda la informacion del fd leida durante la ejecucion del metodo, en caso de no querer esta informacion se pasa NULL
a este parametro*/
int inc_pos_sock_http_request(my_file_http *file, int inc, char **buff_guard);

/*retorna el valor entero que representa la cifra hexadecimal*/
int get_hex_value(char ch);

/*"traduce" el string de la codificacion de porciento a ASCII*/
char* parse_from_percent(char* s);

/*libera el espacio ocupado por el srtuct http_request*/
void free_http_request(http_request* request);

/*libera la memoria usada por una lista de headers de una peticion*/
void free_headers_list(linked_list *list);

/*escribe un response line con el codigo y el texto indicado al socket en fd*/
void write_error_message(char *code, char *text, int fd);

/*recibe un puntero a una url, un puntero al final de la misma y un fd de un socket
y le escribe la respuesta http para descargar ese archivo*/
void download_file(char *url,char* url_ptr, int connect_fd);

#endif // HTTP_TOOLS_GUARD
