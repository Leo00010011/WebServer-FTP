#include "sorthtml.h"
#include "genhtml.h"
#include "http_tools.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

char **str_row(mystat *mystat)
{
    char *url = NULL; int leng = strlen(mystat->url);
    char **row = calloc(4, sizeof(char *)); int index = 0;

    //name
    if (S_ISDIR(mystat->stat->st_mode))
    {
        url = realloc(url, (leng + 12) * sizeof(char));
        index = my_strcopy(url, "sortbyname?", index);
        index = my_strcopy(url, mystat->url, index);
        url[index] = '\0';
        row[0] = GenLink(url, mystat->name);
        free(url);
    }
    else
    {
        url = realloc(url, (leng + 1) * sizeof(char));
        index = my_strcopy(url, mystat->url, index);
        url[index] = '\0';
        row[0] = GenLink(url, mystat->name);
        free(url);
    }

    if (strcmp(mystat->name, ".."))
    {
        //size
        row[1] = calloc(((mystat->stat->st_size) ? (int)log10(mystat->stat->st_size) + 1 : 1), sizeof(char));
        sprintf(row[1], "%i b", mystat->stat->st_size);
        //date
        struct tm *time = calloc(1, sizeof(struct tm));
        row[2] = calloc(__TIMESIZE, sizeof(char));
        time = localtime_r(&(mystat->stat->st_mtime), time);
        strftime(row[2], __TIMESIZE, "%F", time);
        free(time);
    }
    else
    {
        row[2] = "";
    }
    
    return row;
}

char **str_head_row(const char *dir)
{
    int i = 0, index;
    int leng = strlen(dir);
    char **row = calloc(4, sizeof(char *));
    char *url = NULL;
    
    //name
    index = 0;
    url = realloc(url, (leng + 12) * sizeof(char));
    index = my_strcopy(url, "sortbyname?", index);
    index = my_strcopy(url, (char *)dir, index);
    url[index] = '\0';
    row[i++] = GenLink(url, "name");
    free(url);
    url = NULL;

    //size
    index = 0;
    url = realloc(url, (leng + 12) * sizeof(char));
    index = my_strcopy(url, "sortbysize?", index);
    index = my_strcopy(url, (char *)dir, index);
    url[index] = '\0';
    row[i++] = GenLink(url, "size");
    free(url);
    url = NULL;

    //date
    index = 0;
    url = realloc(url, (leng + 12) * sizeof(char));
    index = my_strcopy(url, "sortbytimem?", index);
    index = my_strcopy(url, (char *)dir, index);
    url[index] = '\0';
    row[i++] = GenLink(url, "time");

    free(url);
    return row;
}

char ***str_table(linked_list *list, const char *dir)
{
    char ***table;
    linked_node *node;
    int i = 0;
    table = calloc(list->count + 2, sizeof(char **));
    table[i++] = str_head_row(dir);
    for (node = list->first; node != NULL; node = node->next, i++)
        table[i] = str_row((mystat *)node->value);
    return table;
}

linked_list *get_list_stat(DIR *direc, const char *dir)
{
    linked_list *list;
    char *actualdir;
    char *name;
    char *temp;
    mystat *mystats;
    struct dirent *direntry;
    struct stat *stats;
    int actualdirindex;
    int lengname;

    list = create_linked_list();
    for (direntry = readdir(direc); direntry != NULL; direntry = readdir(direc))
    {
        if (!strcmp(direntry->d_name,"."))
            continue;
        actualdirindex = 0;
        actualdir = calloc((strlen(dir) + strlen(direntry->d_name) + 2), sizeof(char));
        actualdirindex = my_strcopy(actualdir, (char *)dir, actualdirindex);
        if (actualdir[actualdirindex - 1] == '/')
            actualdir[actualdirindex--] = '\0';
        temp = strrchr(actualdir, '/');
        if (!strcmp(direntry->d_name, "..") && temp != NULL)
        {
            *temp = '\0';
            actualdirindex = strlen(actualdir);
            actualdir = realloc(actualdir, actualdirindex + 1);
            if(!actualdirindex)
            {
                free(actualdir);
                actualdir = "/";
            }
        }
        else
        {
            actualdirindex = my_strcopy(actualdir, "/", actualdirindex);
            actualdirindex = my_strcopy(actualdir, direntry->d_name, actualdirindex);
            actualdir[actualdirindex] = '\0';
        }

        stats = calloc(1, sizeof(struct stat));
        stat(actualdir, stats);

        lengname = 0;
        name = calloc(strlen(direntry->d_name) + 1, sizeof(char));
        lengname = my_strcopy(name, direntry->d_name, lengname);
        name[lengname] = '\0';

        mystats = calloc(1, sizeof(mystat));
        mystats->name = name;
        mystats->stat = stats;
        mystats->url = actualdir;
        add_last(list, mystats);
        temp = NULL;
    }

    return list;
}

char *GetTruecwd(char *cwd)
{

}

int SendNotFound()
{
    http_response response;
    response.status_c = "404";
    response.status_m = "Not found";
    response.version = "HTTP/1.1";
    response.content = "\r\n";
    response.headers = NULL;
    response.len = 26;
    return write_response_small_content(&response, 1);
}

int SendHTML(char *html)
{
    http_response response;
    response.status_c = "200";
    response.status_m = "OK";
    response.version = "HTTP/1.1";

    response.headers = create_linked_list();
    pair_key_value* pair = malloc(sizeof(pair_key_value));

    pair->key = "Content-Type";
    pair->value = "text/html";
    add_last(response.headers,pair);

    pair = malloc(sizeof(pair_key_value));
    pair->key = "Content-Length";
    pair->value = malloc(1);
    sprintf(pair->value, "%d", strlen(html));
    add_last(response.headers,pair);

    response.content = html;
    fill_response_len(&response);
    return write_response_small_content(&response, 1);
}