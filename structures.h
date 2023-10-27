#ifndef STRUCTURES_GUARD_
#define STRUCTURES_GUARD_
#include <stdlib.h>
typedef struct linked_node
{
    struct linked_node * preview;
    struct linked_node * next;
    void * value;
}linked_node;

typedef struct linked_list
{
    linked_node * first;
    linked_node * last;
    int count;
}linked_list;

typedef struct 
{
    linked_node * top;
    int count;
}stack;

/*fixed size queue
**cuando se supera el tamanno fijado de la cola borra los elementos mas viejos **/
typedef struct 
{
    void** array;
    int size;
    int first_index;
    int count;
}fs_queue;


typedef struct
{
    void* key;
    void* value;
}pair_key_value;




/*inicializa una linked list
el preview del first siempre va a apuntar a NULL
el next del last siempre va a apuntar a NULL*/
linked_list * create_linked_list();
//guarda el puntero al valor al principio de la lista
void add_first(linked_list * list, void * value);
//guarda el puntero al valor al final de la lista
void add_last(linked_list * list, void * value);
/*remueve el primer linked_node de la lista y libera la memoria que ocupa
**libera la memoria que ocupa el nodo pero no la del valor**/
void remove_first(linked_list * list);
/*remueve el primer linked_node de la lista y libera la memoria que ocupa
**libera la memoria que ocupa el nodo pero no la del valor**/
void remove_last(linked_list * list);

void remove_node(linked_list* list,linked_node* node);

void free_linked_list(linked_list* list);

/*inicializa un stack*/
stack * create_stack();
/*hace pop a la pila
**libera la memoria que ocupa el nodo pero no la del valor**/
void * pop(stack* stack);
/*hace push al valor en la pila*/
void push(stack* stack,void* value);
/*inicializa un queue de tamanno fijo*/
fs_queue* create_fs_queue(int size);

void* get_fs_queue(fs_queue* queue, int index);

void* dequeue_fs(fs_queue* queue);

int enqueue_fs(fs_queue* queue,void* value);


char* get_value_str(linked_list* list,char* key);

void set_value_str(linked_list* list,char* key,char* value);

int remove_pair_str(linked_list* list, char* key);

linked_node *find_index(linked_list *list, int index);

linked_list *sort_by_index(linked_list *list, int (*comparer)(void *stat1, void *stat2), int index);

linked_list *sort_by(linked_list *list, int (*comparer)(void *stat1, void *stat2));

#endif