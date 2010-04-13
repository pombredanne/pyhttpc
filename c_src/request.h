#ifndef PYHTTPC_REQUEST_H
#define PYHTTPC_REQUEST_H

#include "pyhttpc.h"
#include "buffer.h"

typedef struct {
    PyObject_HEAD
    PyObject* parser;

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

    PyObject*   body;
} Request;

typedef struct {
    PyObject_HEAD
    int         cs;
    
    PyObject*   source;
    PyObject*   current;
    const char* buffer;
    const char* bufpos;
    Py_ssize_t  buflen;
    
    Py_ssize_t  nread;

    Request*    request;
    
    buffer_t*   genbuf;
    buffer_t*   uribuf;
} RequestParser;

#endif // Included request.h