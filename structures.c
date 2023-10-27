
#include "structures.h"
#include <string.h>

linked_list *create_linked_list()
{
    linked_list *new_list = (linked_list *)malloc(sizeof(linked_list));
    new_list->first = NULL;
    new_list->last = NULL;
    new_list->count = 0;
    return new_list;
}

void add_first(linked_list *list, void *value)
{
    linked_node *new_node = (linked_node *)malloc(sizeof(linked_node));
    new_node->value = value;
    if (list->count == 0)
    {
        list->first = new_node;
        list->last = new_node;
        list->count = 0;
        new_node->preview = NULL;
        new_node->next = NULL;
    }
    else
    {
        new_node->next = list->first;
        new_node->preview = NULL;
        list->first->preview = new_node;
        list->first = new_node;
    }
    list->count++;
}

void add_last(linked_list *list, void *value)
{
    linked_node *new_node = malloc(sizeof(linked_node));
    new_node->value = value;
    if (list->count == 0)
    {
        list->first = new_node;
        list->last = new_node;
        new_node->preview = NULL;
        new_node->next = NULL;
    }
    else
    {
        new_node->preview = list->last;
        new_node->next = NULL;
        list->last->next = new_node;
        list->last = new_node;
    }
    list->count++;
}

void remove_first(linked_list *list)
{
    if (list->count == 0)
    {
        return;
    }
    else
    {
        if (list->count == 1)
        {
            free(list->first);
            list->first = NULL;
            list->last = NULL;
        }
        else
        {
            list->first = list->first->next;
            free(list->first->preview);
            list->first->preview = NULL;
        }

        list->count--;
    }
}

void remove_last(linked_list *list)
{
    if (list->count == 0)
    {
        return;
    }
    else
    {
        if (list->count == 1)
        {
            free(list->first);
            list->first = NULL;
            list->last = NULL;
        }
        else
        {
            list->last = list->last->preview;
            free(list->last->next);
            list->last->next = NULL;
        }

        list->count--;
    }
}

void remove_node(linked_list *list, linked_node *node)
{
    if (node->preview != NULL)
    {
        node->preview->next = node->next;
    }
    else
    {
        list->first = node->next;
    }

    if (node->next != NULL)
    {
        node->next->preview = node->preview;
    }
    else
    {
        list->last = node->preview;
    }
    list->count--;
    free(node);
}

stack *create_stack()
{
    stack *new_stack = (stack *)malloc(sizeof(stack));
    new_stack->top = NULL;
    new_stack->count = 0;
    return new_stack;
}

void *pop(stack *stack)
{
    if (stack->count == 0)
    {
        return NULL;
    }
    else
    {
        void *value = stack->top->value;
        if (stack->top->preview != NULL)
        {
            stack->top = stack->top->preview;
            free(stack->top->next);
            stack->top->next = NULL;
        }
        else
        {
            free(stack->top);
            stack->top = NULL;
        }
        stack->count--;
        return value;
    }
}

void push(stack *stack, void *value)
{
    linked_node *new_node = (linked_node *)malloc(sizeof(linked_node));
    new_node->value = value;
    if (stack->count != 0)
    {
        stack->top->next = new_node;
        new_node->preview = stack->top;
    }
    stack->top = new_node;
    stack->count++;
}

fs_queue *create_fs_queue(int size)
{
    fs_queue *new_fs_queue = malloc(sizeof(fs_queue));
    new_fs_queue->array = malloc(size * sizeof(void *));
    new_fs_queue->first_index = 0;
    new_fs_queue->size = size;
    return new_fs_queue;
}

void *get_fs_queue(fs_queue *queue, int index)
{
    if (index >= queue->size)
    {
        return NULL;
    }
    return queue->array[(queue->first_index + index) % queue->size];
}

void *dequeue_fs(fs_queue *queue)
{
    void *value = queue->array[queue->first_index];
    queue->count--;
    queue->first_index = (queue->first_index + 1) % (queue->size);
    return value;
}

int enqueue_fs(fs_queue *queue, void *value)
{
    if (queue->count == queue->size)
    {
        queue->array[queue->first_index] = value;
        queue->first_index = (queue->first_index + 1) % queue->size;
        return 1;
    }
    else
    {
        queue->array[(queue->first_index + queue->count) % queue->size] = value;
        queue->count++;
        return 0;
    }
}

char *get_value_str(linked_list *list, char *key)
{
    linked_node *current = list->first;
    while (current != NULL)
    {
        if (strcmp(key, (char *)((pair_key_value *)current->value)->key) == 0)
        {
            return (char *)((pair_key_value *)current->value)->value;
        }
        current = current->next;
    }
}

void set_value_str(linked_list *list, char *key, char *value)
{
    linked_node *current = list->first;
    while (current != NULL)
    {
        if (strcmp(key, (char *)((pair_key_value *)current->value)->key) == 0)
        {
            ((pair_key_value *)current->value)->value = value;
            return;
        }
        current = current->next;
    }
    pair_key_value *new_pair = malloc(sizeof(pair_key_value));
    new_pair->key = key;
    new_pair->value = value;
    add_last(list, new_pair);
    return;
}

int remove_pair_str(linked_list *list, char *key)
{
    linked_node *current = list->first;
    while (current != NULL)
    {
        if (strcmp(key, (char *)(((pair_key_value *)current->value)->key)) == 0)
        {
            remove_node(list, current);
            return 0;
        }
        current = current->next;
    }
    return -1;
}

void free_linked_list(linked_list* list)
{
    linked_node* current = list->first;
    while (current != NULL)
    {
        free(current->value);
        if(current->next == NULL)
        {
            free(current);
            break;
        }
        current = current->next;
        free(current->preview);
    }
    free(list);
}

linked_node *find_index(linked_list *list, int index)
{
    if (index > list->count - 1)
        return NULL;
    linked_node *node = list->first;
    for (int i = 0; i < index; i++)
        node = node->next;
    return node;
}

linked_list *sort_by_index(linked_list *list, int (*comparer)(void *stat1, void *stat2), int index)
{
    for (int i = index; i < list->count; i++)
    {
        for (linked_node *node = find_index(list, index)->next; node != NULL; node = node->next)
        {
            if (comparer(node->value, node->preview->value) < 0)
            {
                linked_node *preview = node->preview;

                if (preview->preview != NULL)
                    preview->preview->next = node;
                else
                    list->first = node;
                if (node->next != NULL)
                    node->next->preview = preview;
                else
                    list->last = preview;
                preview->next = node->next;
                node->preview = preview->preview;
                preview->preview = node;
                node->next = preview;

                node = node->next;
            }
        }
    }
    return list;
}

linked_list *sort_by(linked_list *list, int (*comparer)(void *stat1, void *stat2))
{
    return sort_by_index(list, comparer, 0);
}

