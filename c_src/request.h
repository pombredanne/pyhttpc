#ifndef PYHTTPC_REQUEST_H
#define PYHTTPC_REQUEST_H

#include <Python.h>

typedef struct {
    const char* pos;
    char*       buf;
    size_t      len;
    size_t      used;
} mark_buf_t;

typedef struct {
    int         cs;
    int         error;

    const char* body;
    size_t      nread;

    PyObject*   fields;
    int         port;
    int         vsn_major;
    int         vsn_minor;

    PyObject*   headers;
    PyObject*   hdr_name;

    mark_buf_t* mark;
    mark_buf_t* mark_uri;
} http_req_parser_t;

#define PARSE_ERROR 0
#define PARSE_OK 1
#define PARSE_MORE 2

int init_req_parser(http_req_parser_t* parser);
void free_req_parser(http_req_parser_t* parser);
int exec_req_parser(http_req_parser_t* parser, const char* buf, size_t len);
const char* get_req_error(http_req_parser_t* parser);

#endif // Included request.h