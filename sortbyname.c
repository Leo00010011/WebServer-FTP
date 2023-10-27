#include "sortby.h"
#include <stdio.h>
#include <string.h>

int compare(void *stat1, void *stat2){
    bool isdir1 = S_ISDIR(((mystat *)stat1)->stat->st_mode);
    bool isdir2 = S_ISDIR(((mystat *)stat2)->stat->st_mode);
    if ((isdir1 && !isdir2) || !strcmp(((mystat *)stat1)->name, ".."))
    {
        return -1;
    }
    if ((!isdir1 && isdir2) || !strcmp(((mystat *)stat2)->name, ".."))
    {
        return 1;
    }
    return strcmp(((mystat *)stat1)->name, ((mystat *)stat2)->name);
}

int main(int argc, char const *argv[])
{
    DIR *direc;
    linked_list *list;
    char ***table;
    char *html;
    
    if (argc != 2)
    {
        fprintf(stderr, "format: sorthtml <dir>\n");
        exit(-1);
    }
    if (!(direc = opendir(argv[1])))
    {
        fprintf(stderr, "error openin %s\n", argv[1]);
        SendNotFound();
        exit(-1);
    }

    list = get_list_stat(direc, argv[1]);
    list = sort_by(list, compare);
    table = str_table(list, argv[1]);
    html = GenHTML(argv[1], table);
    SendHTML(html);

    return 0;
}