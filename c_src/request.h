#ifndef PYHTTPC_REQUEST_H
#define PYHTTPC_REQUEST_H

#include "pyhttpc.h"
#include "buffer.h"

enum body_func {
    bt_error=0,
    bt_chunked,
    bt_length,
    bt_eof
};

typedef struct {
    PyObject_HEAD
    struct _RequestParser* parser;

    PyObject*   method;

    PyObject*   uri;
    PyObject*   scheme;
    PyObject*   host;
    int         port;
    PyObject*   path;
    PyObject*   query;
    PyObject*   fragment;
    
    PyObject*   version;
    int         vsn_major;
    int         vsn_minor;

    PyObject*   headers;
    PyObject*   hdr_name;

    int         body_type;
    int         body_len;
} Request;


typedef struct _RequestParser {
    PyObject_HEAD
    int         cs;
    
    PyObject*   source;
    PyObject*   current;
    char*       buffer;
    char*       bufpos;
    Py_ssize_t  buflen;
    
    Py_ssize_t  nread;

    Request*    request;
    
    buffer_t*   genbuf;
    buffer_t*   uribuf;
} RequestParser;

#endif // Included request.h