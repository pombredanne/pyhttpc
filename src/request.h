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
} RequestParser;

int init_req_parser(RequestParser* parser);
void free_req_parser(RequestParser* parser);
size_t exec_req_parser(RequestParser* parser, const char* buffer, size_t len);
char* get_req_error(RequestParser* parser);

#endif // Included request.h