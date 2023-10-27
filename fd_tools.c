#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "fd_tools.h"

void save_var(int fd, void *buffer, unsigned int size)
{
    write(fd, &size, sizeof(unsigned int));
    write(fd, buffer, size);
}

void *load_var(int fd, int *result)
{
    int temp;ö
    unsigned int size;
    read(fd, &size, sizeof(unsigned int));
    void *var = malloc(size);
    temp = read(fd, var, size);
    if (result != NULL)
    {
        *result = temp;
    }
    return var;
}

int refresh_file(my_file *file, int size)
{
    int readed;
    if ((readed = read_rio(file->fd, file->buff, size)) < 0)
    {
        return -1;
    }
    file->buff_size = readed;
    file->buff_pos = 0;
    return 0;
}

int read_rio(int fd, char *buffer, int size)
{
    int left = size;
    int readed;
    char *bufp = buffer;
    while (left > 0)
    {
        if ((readed = read(fd, bufp, size)) < 0)
        {
            if (errno = EINTR)
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
            if (readed == 0)
            {
                break;
            }
        }
        left -= readed;
        bufp += readed;
    }
    return size - left;
}

int write_rio(int fd, char *buffer, int size)
{
    int left = size;
    int writen;
    char *bufp = buffer;
    while (left > 0)
    {
        if ((writen = write(fd, bufp, size)) < 0)
        {
            if (errno = EINTR)
            {
                writen = 0;
            }
            else
            {
                return -1;
            }
        }
        left -= writen;
        bufp += writen;
    }
    return size;
}
