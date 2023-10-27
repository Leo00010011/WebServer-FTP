#include "genhtml.h"
#include "http_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *GetTitle(const char *strtitle)
{
    char *urlname = strrchr(strtitle, '\\');
    urlname = (urlname != NULL) ? urlname + 1 : (char *)strtitle;
    char *title = calloc((strlen(urlname) + TITLE_TAG_LENG__ + 1), sizeof(char));

    title = strcat(title, "<title>");
    title = strcat(title, urlname);
    title = strcat(title, "</title>\n");
    return title;
}

char *GenHead(const char *strtitle)
{
    char *title = GetTitle(strtitle);
    char *metach = "<meta charset=\"utf-8\">\n";
    char *head = calloc((strlen(metach) + strlen(title) + HEAD_TAG_LENG__ + 1), sizeof(char));

    head = strcat(head, "<head>\n");
    head = strcat(head, metach);
    head = strcat(head, title);
    head = strcat(head, "</head>\n");

    free(title);
    return head;
}

char *GenLink(char *urlcode, char *urlname)
{
    char *link = calloc((strlen(urlcode) + strlen(urlname) + HEAD_TAG_LENG__ + 1), sizeof(char));

    link = strcat(link, "<A HREF=\"");
    link = strcat(link, urlcode);
    link = strcat(link, "\">");
    link = strcat(link, urlname);
    link = strcat(link, "</A>");

    return link;
}

char *GenCols(char **rowhtml)
{
    int size = 1;
    char *cols = NULL;
    for (int i = 0, index = 0; rowhtml[i] != NULL; i++)
    {
        size += strlen(rowhtml[i]) + TDH_TAG_LENG__;
        cols = realloc(cols, size);
        index = my_strcopy(cols, "<td>", index);
        index = my_strcopy(cols, rowhtml[i], index);
        index = my_strcopy(cols, "</td>\n", index);
    }
    cols[size - 1] = '\0';
    return cols;
}

char *GenRows(char ***tablehtml)
{
    int size = 1;
    char *rows = NULL, *cols = NULL;
    for (int i = 0, index = 0; tablehtml[i] != NULL; i++)
    {
        cols = GenCols(tablehtml[i]);
        size += strlen(cols) + TR_TAG_LENG__;
        rows = realloc(rows, size);
        index = my_strcopy(rows, "<tr>\n", index);
        index = my_strcopy(rows, cols, index);
        index = my_strcopy(rows, "</tr>\n", index);
        free(cols);
        cols = NULL;
    }
    rows[size - 1] = '\0';
    return rows;
}

char *GenTable(char ***tablehtml)
{
    char *rows = GenRows(tablehtml);
    char *table = calloc((strlen(rows) + TABLE_TAG_LENG__ + 1), sizeof(char));

    table = strcat(table, "<table>\n");
    table = strcat(table, rows);
    table = strcat(table, "</table>\n");

    free(rows);
    return table;
}

char *GenBody(char ***tablehtml)
{
    char *table = GenTable(tablehtml);
    char *body = calloc((strlen(table) + BODY_TAG_LENG__ + 1), sizeof(char));

    body = strcat(body, "<body>\n");
    body = strcat(body, table);
    body = strcat(body, "</body>\n");

    free(table);
    return body;
}

char *GenHTML(const char *strtitle, char ***tablehtml)
{
    char *head = GenHead(strtitle);
    char *body = GenBody(tablehtml);
    char *html = calloc((strlen(body) + strlen(head) + BODY_TAG_LENG__ + 1), sizeof(char));

    html = strcat(html, "<html>\n");
    html = strcat(html, head);
    html = strcat(html, body);
    html = strcat(html, "</html>\n");

    free(head);
    free(body);
    return html;
}