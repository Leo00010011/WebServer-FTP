#if !defined(SORT_HTML_HEADER__)
#define SORT_HTML_HEADER__

#include "structures.h"
#include "http_tools.h"
#include <sys/stat.h>
#include <dirent.h>

typedef struct mystat
{
    char *url;
    char *name;
    struct stat *stat;
} mystat;

int SendNotFound();
int SendHTML(char *html);
char **str_row(mystat *mystat);
char **str_head_row(const char *dir);
char ***str_table(linked_list *list, const char *dir);
linked_list *get_list_stat(DIR *direc, const char *dir);
#endif // SORT_HTML_HEADER__