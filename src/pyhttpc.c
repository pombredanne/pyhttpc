
#include <Python.h>
#include "structmember.h"
#include "http-parser/http_parser.h"

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    PyObject* method;
    PyObject* path;
    PyObject* query_string;
    PyObject* headers;
    PyObject* body;
    char is_final;
} Request;

PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Request* self = NULL;

    self = (Request*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;
    
    self->headers = PyList_New(0);
    if(self->headers == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    
    self->method = NULL;
    self->path = NULL;
    self->query_string = NULL;
    self->body = NULL;
    self->is_final = 0;

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

void
Request_dealloc(Request* self)
{
    Py_XDECREF(self->method);
    Py_XDECREF(self->path);
    Py_XDECREF(self->query_string);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->body);
}

static PyMemberDef Request_members[] = {
    {"method", T_OBJECT, offsetof(Request, method), READONLY,
        "HTTP Method"},
    {"path", T_OBJECT, offsetof(Request, path), READONLY,
        "HTTP Path"},
    {"query_string", T_OBJECT, offsetof(Request, query_string), READONLY,
        "HTTP Query String"},
    {"headers", T_OBJECT, offsetof(Request, headers), READONLY,
        "HTTP Headers as a list of tuples."},
    {"body", T_OBJECT, offsetof(Request, body), READONLY,
        "HTTP Body"},
    {"is_final", T_BOOL, offsetof(Request, is_final), READONLY,
        "Should we close the connection after this request?"},
    {NULL}
};

static PyMethodDef Request_methods[] = {
    {NULL}
};

PyTypeObject RequestType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.Request",                          /*tp_name*/
    sizeof(Request),                            /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)Request_dealloc,                /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash*/
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    "PyHttpC Request",                          /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    0,		                                    /*tp_iter*/
    0,		                                    /*tp_iternext*/
    Request_methods,                            /*tp_methods*/
    Request_members,                            /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    Request_new,                                /*tp_new*/
};

enum Callback {
    INIT_PARSER         = 1,
    ON_MESSAGE_START    = 2,
    ON_METHOD           = 4,
    ON_PATH             = 8,
    ON_QUERY_STRING     = 16,
    ON_HEADER_FIELD     = 32,
    ON_HEADER_VALUE     = 64,
    ON_HEADERS_COMPLETE = 128,
    ON_BODY             = 256,
    ON_MESSAGE_COMPLETE = 512
};

typedef struct {
    PyObject_HEAD
    http_parser* parser;
    Request* req;
    char* field;
    char* buf;
    size_t buflen;
    size_t bufused;
    int prevcb;
} Parser;

// 80K matching http-parser's max field value.
#define MAX_BUF_LEN 81920

#define CHECK_PARSER(P) \
    if((P) == NULL) {PyErr_BadInternalCall(); return -1;}
#define CHECK_STATE(P, S) \
    if(((P)->prevcb & (S)) != (P)->prevcb) {PyErr_BadInternalCall(); return -1;}
#define RESET_BUF(P) \
    memset((P)->buf, 0, (P)->buflen); (P)->bufused = 0;
 
static int
copy_buf(Parser* p, const char* buf, size_t length)
{
    char* tmp = NULL;
    while(p->bufused + length + 1 > p->buflen) // +1 for the trailing \0
    {        
        if(p->buflen > MAX_BUF_LEN)
        {
            PyErr_SetString(PyExc_RuntimeError, "Buffer too large.");
            return -1;
        }
        
        tmp = p->buf;
        p->buf = (char*) malloc(p->buflen * 2 * sizeof(char));
        if(p->buf == NULL)
        {
            p->buf = tmp;
            PyErr_NoMemory();
            return -1;
        }
        
        memcpy(p->buf, tmp, p->bufused);
        p->buflen *= 2;

        // Zero out new buffer space
        memset(p->buf+p->bufused, 0, p->buflen - p->bufused);
        
        free(tmp);
    }
    
    strncat(p->buf, buf, length);
    p->bufused += length;
    return 0;
}

static int
on_message_begin(http_parser* parser)
{
    Parser* self = (Parser*) parser->data;
    CHECK_PARSER(self);
    CHECK_STATE(self, INIT_PARSER);

    Py_XDECREF(self->req);
    
    self->req = \
        (Request*) PyObject_CallObject((PyObject*) &RequestType, NULL);
    if(self->req == NULL) return -1;

    self->prevcb = ON_MESSAGE_START;

    return 0;
}

static int
on_method(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_MESSAGE_START | ON_METHOD);
    self->prevcb = ON_METHOD;

    return copy_buf(self, at, length);
}

static int
on_path(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    PyObject* method = NULL;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_METHOD | ON_PATH);
    
    if(self->prevcb == ON_METHOD)
    {
        method = PyString_FromString(self->buf);
        if(method == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }
        
        assert(self->req->method == NULL);
        self->req->method = method;
        RESET_BUF(self);
    }
    
    self->prevcb = ON_PATH;
    
    return copy_buf(self, at, length);
}

static int
on_query_string(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    PyObject* path = NULL;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_PATH | ON_QUERY_STRING);
    
    if(self->prevcb == ON_PATH)
    {
        path = PyString_FromString(self->buf);
        if(path == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }
        
        assert(self->req->path == NULL);
        self->req->path = path;
        RESET_BUF(self);
    }
    
    self->prevcb = ON_QUERY_STRING;
    
    return copy_buf(self, at, length);
}

static int
on_header_field(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    PyObject* query_string = NULL;
    PyObject* pair = NULL;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_QUERY_STRING | ON_HEADER_FIELD | ON_HEADER_VALUE);

    if(self->prevcb == ON_QUERY_STRING)
    {
        query_string = PyString_FromString(self->buf);
        if(query_string == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }
        
        assert(self->req->query_string == NULL);
        self->req->query_string = query_string;
        RESET_BUF(self);
    }
    else if(self->prevcb == ON_HEADER_VALUE)
    {
        pair = Py_BuildValue("(ss)", self->field, self->buf);
        if(pair == NULL) return -1;

        if(PyList_Append(self->req->headers, pair) != 0)
        {
            Py_DECREF(pair);
            return -1;
        }
        
        free(self->field);
        self->field = NULL;
        RESET_BUF(self);
    }
    
    self->prevcb = ON_HEADER_FIELD;
    
    return copy_buf(self, at, length);
}

static int
on_header_value(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_HEADER_FIELD | ON_HEADER_VALUE);

    if(self->prevcb == ON_HEADER_FIELD)
    {
        self->field = (char*) malloc((self->bufused+1) * sizeof(char));
        if(self->field == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }

        memcpy(self->field, self->buf, self->bufused+1);
        RESET_BUF(self);
    }
    
    self->prevcb = ON_HEADER_VALUE;
    
    return copy_buf(self, at, length);
}

static int
on_headers_complete(http_parser* parser)
{
    Parser* self = (Parser*) parser->data;
    PyObject* pair = NULL;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_HEADER_VALUE);

    pair = Py_BuildValue("(ss)", self->field, self->buf);
    if(pair == NULL) return -1;

    if(PyList_Append(self->req->headers, pair) != 0)
    {
        Py_DECREF(pair);
        return -1;
    }
    
    free(self->field);
    self->field = NULL;
    RESET_BUF(self);
    
    self->prevcb = ON_HEADERS_COMPLETE;
    
    return 0;
}

static int
on_body(http_parser* parser, const char* at, size_t length)
{
    Parser* self = (Parser*) parser->data;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_HEADERS_COMPLETE | ON_BODY);
    self->prevcb = ON_BODY;
    return copy_buf(self, at, length);
}

static int
on_message_complete(http_parser* parser)
{
    Parser* self = (Parser*) parser->data;
    PyObject* body = NULL;
    CHECK_PARSER(self);
    CHECK_STATE(self, ON_HEADERS_COMPLETE | ON_BODY);

    if(self->prevcb == ON_BODY)
    {
        body = PyString_FromString(self->buf);
        if(body == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }
        
        assert(self->req->body == NULL);
        self->req->body = body;
        RESET_BUF(self);
    }
    
    self->prevcb = ON_MESSAGE_COMPLETE;
    self->req->is_final = http_should_keep_alive(parser) ? 1 : 0;
    return 0;
}

PyObject*
Parser_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Parser* self = NULL;

    self = (Parser*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;
    self->buf = NULL;

    self->parser = (http_parser*) malloc(sizeof(http_parser));
    if(self->parser == NULL) goto error;

    self->req = NULL;
    self->field = NULL;
    self->prevcb = INIT_PARSER;

    self->bufused = 0;        
    self->buflen = 4096; // Should make this configurable.
    self->buf = (char*) malloc(self->buflen * sizeof(char));
    if(self->buf == NULL)
    {
        goto error;
    }
    
    http_parser_init(self->parser);
    self->parser->data = (void*) self;
    self->parser->on_message_begin = on_message_begin;
    self->parser->on_method = on_method;
    self->parser->on_path = on_path;
    self->parser->on_query_string = on_query_string;
    self->parser->on_header_field = on_header_field;
    self->parser->on_header_value = on_header_value;
    self->parser->on_headers_complete = on_headers_complete;
    self->parser->on_body = on_body;
    self->parser->on_message_complete = on_message_complete;

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

void
Parser_dealloc(Parser* self)
{
    if(self->parser) free(self->parser);
    if(self->buf) free(self->buf);
    if(self->field) free(self->field);
    Py_XDECREF(self->req);
}

PyDoc_STRVAR(Parser_parse_requests__doc__,
"Parser.parse_requests(data)\n\n\
Parse HTTP requests from data. Data should be an \
instance of str.");

static PyObject*
Parser_parse_requests(Parser* self, PyObject* args, PyObject* kwargs)
{
    char* data = NULL;
    int len = 0;
    size_t resp = 0;

    if(!PyArg_ParseTuple(args, "s#", &data, &len)) return NULL;
    resp = http_parse_requests(self->parser, data, (size_t) len);
    
    return PyInt_FromLong((long) resp);
}

PyDoc_STRVAR(Parser_parse_responses__doc__,
"Parser.parse_responses(data)\n\n\
Parse HTTP responses from data. Data should be an \
instance of str.");

static PyObject*
Parser_parse_responses(Parser* parser, PyObject* args, PyObject* kwargs)
{
    char* data = NULL;
    int len = 0;
    size_t resp = 0;
    if(!PyArg_ParseTuple(args, "s#", &data, &len)) return NULL;
    resp = http_parse_responses(parser->parser, data, (size_t) len);
    return PyInt_FromLong((long) resp);
}

PyDoc_STRVAR(Parser_should_keep_alive__doc__,
"Parser.should_keep_alive() -> bool\n\n\
Determine if this should be the last message in an\n\
HTTP stream. You should only call this from on_headers_complete\n\
or on_body_complete.");

static PyObject*
Parser_should_keep_alive(Parser* parser, PyObject* args, PyObject* kwargs)
{
    if(http_should_keep_alive(parser->parser))
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyMemberDef Parser_members[] = {
    {"request", T_OBJECT, offsetof(Parser, req), READONLY, "HTTP Request"},
    {"state", T_INT, offsetof(Parser, prevcb), READONLY, "Last parse state."},
    {NULL}
};

static PyMethodDef Parser_methods[] = {
    {"parse_requests", (PyCFunction)Parser_parse_requests,
        METH_VARARGS, Parser_parse_requests__doc__},
    {"parse_responses", (PyCFunction)Parser_parse_responses,
        METH_VARARGS, Parser_parse_responses__doc__},
    {"should_keep_alive", (PyCFunction)Parser_should_keep_alive,
        METH_NOARGS, Parser_should_keep_alive__doc__},
    {NULL}
};

PyTypeObject ParserType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.Parser",                           /*tp_name*/
    sizeof(Parser),                             /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)Parser_dealloc,                 /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash*/
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    "PyHttpC Parser",                           /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    0,		                                    /*tp_iter*/
    0,		                                    /*tp_iternext*/
    Parser_methods,                             /*tp_methods*/
    Parser_members,                             /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    Parser_new,                                 /*tp_new*/
};

static PyMethodDef pyhttpc_methods[] = {
    {NULL}
};

PyMODINIT_FUNC
initpyhttpc(void)
{
    PyObject* m;

    if(PyType_Ready(&RequestType) < 0) return;
    if(PyType_Ready(&ParserType) < 0) return;

    m = Py_InitModule3("pyhttpc", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&ParserType);
    Py_INCREF(&RequestType);
    
    PyModule_AddObject(m, "Parser", (PyObject*) &ParserType);

    PyHttpCModule = m;
}
