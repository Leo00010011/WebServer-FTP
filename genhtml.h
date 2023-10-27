#if !defined(GEN_HTML_HEADER___)

#define GEN_HTML_HEADER___
#define TITLE_TAG_LENG__ 16
#define HEAD_TAG_LENG__ 15
#define LINK_TAG_LENG__ 15
#define TDH_TAG_LENG__ 10
#define TR_TAG_LENG__ 11
#define TABLE_TAG_LENG__ 17
#define BODY_TAG_LENG__ 15
#define HTML_TAG_LENG__ 15

//genera un <title> a partir de un urlcode
char *GetTitle(const char *urlcode);

//genera un <head> a partir de un urlcode
char *GenHead(const char *urlcode);

//genera un <link> a partir de un urlcode
char *GenLink(char *urlcode, char *urlname);

//genera un <row> a partir de un urlcode
char *GenCols(char **strcols);

//genera una tabla de <row> a partir de un urlcode
char *GenRows(char ***strrows);

//genera un <table> a partir de un urlcode
char *GenTable(char ***strrows);

//genera un <body> a partir de un urlcode
char *GenBody(char ***strrows);

//genera un <html> a partir de un urlcode
char *GenHTML(const char *urlcode, char ***strrows);
#endif // GEN_HTML_HEADER___
