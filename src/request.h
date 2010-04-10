#ifndef PYHTTPC_REQUEST_H
#define PYHTTPC_REQUEST_H

#include <Python.h>

typedef struct {
    int         cs;
    int         error;

    const char* body;
    size_t      nread;

    char*       method;
    char*       uri;
    char*       scheme;
    char*       host;
    int         port;
    char*       path;
    char*       query;
    char*       fragment;

    int         vsn_major;
    int         vsn_minor;

    PyObject*   headers; 
    char*       hdr_name;
    char*       hdr_value;

    const char* mark;
    char*       mark_buf;
    size_t      mark_len;

    const char* mark_uri;
    char*       mark_uri_buf;
    size_t      mark_uri_len;

} RequestParser;

int init_req_parser(RequestParser* parser, PyObject* fetch_func);
size_t exec_req_parser(RequestParser* parser, const char* buffer, size_t len);
char* get_req_error(RequestParser* parser);

#endif // Included request.h