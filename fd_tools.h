#ifndef FD_TOOLS_GUARD
#define FD_TOOLS_GUARD
/*para inicializar:
  - asignarle el fd al que se va a referir
  - llamar refresh con el tamanno de buffer deseado*/
typedef struct my_file
{
  int fd;
  char *buff;
  int buff_size;
  int buff_pos;
} my_file;

/*salva el contenido del buffer en el archivo indicado para ser leido por load_var*/
void save_var(int fd, void *buffer, unsigned int size);

/*carga el contenido de una variable guardada con save_var
  ** el pos del fd debe estar en la primera posicion del entero que indica el tamanno de la variable*
  ** para evitar complicaciones se recomienda solo leer el archivo con load_var y modificarlo con save_var*
  ** si result != NULL en el se pone el valor que retorna el read al intentar leer el archivo* */
void *load_var(int fd, int *result);

/*actualiza buff con nuevos valores correctamente*/
int refresh_file(my_file *file, int size);

/*Lee del fd reiniciando la lectura cuando llega un signal
y terminando manejando correctamente los shortcounts para
leer exactamente <size> bytes*/
int read_rio(int fd, char *buffer, int size);
/*Escribe en fd reiniciando la escritura cuando llega un signal
y terminando manejando correctamente los shortcounts para
escribir exactamente <size> bytes*/
int write_rio(int fd, char *buffer, int size);

//libera todo el espacio ocupado por la estructura y su buffer
void free_file(my_file *file);

#endif
