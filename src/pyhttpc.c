
#include <Python.h>
#include "structmember.h"
#include "http_parser.h"

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    http_parser* parser;
} Parser;

static PyObject*
get_cb(PyObject* self, const char* name)
{
    PyObject* ret = NULL;

    ret = PyObject_GetAttrString((PyObject*) self, name);
    if(ret == NULL)
    {
        PyErr_Clear();
        goto error;
    }

    if(!PyCallable_Check(ret))
    {
        PyErr_SetString(PyExc_TypeError, "callbacks need to be callable");
        goto error;
    }

    goto success;

error:
    Py_XDECREF(ret);
    ret = NULL;

success:
    return ret;
}

static int
with_args(PyObject* self, PyObject* func, const char* at, size_t length)
{
    PyObject* result = NULL;
    if(func == NULL) return 0;
    result = PyObject_CallFunction(func, "s#", at, length);
    if(result == NULL) return 1;
    Py_DECREF(result);
    return 0;
}

static int
no_args(PyObject* self, PyObject* func)
{
    PyObject* result = NULL;
    if(func == NULL) return 0;
    result = PyObject_CallFunction(func, NULL);
    if(result == NULL) return 1;
    Py_DECREF(result);
    return 0;
}

static int
on_message_begin(http_parser* parser)
{
    PyObject* self = (PyObject*) parser->data;
    return no_args(self, get_cb(self, "on_message_begin"));
}

static int
on_path(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_path"), at, length);
}

static int
on_query_string(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_query_string"), at, length);
}

static int
on_url(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_url"), at, length);
}

static int
on_fragment(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_fragment"), at, length);
}

static int
on_header_field(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_header_field"), at, length);
}

static int
on_header_value(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_header_value"), at, length);
}

static int
on_headers_complete(http_parser* parser)
{
    PyObject* self = (PyObject*) parser->data;
    return no_args(self, get_cb(self, "on_headers_complete"));
}

static int
on_body(http_parser* parser, const char* at, size_t length)
{
    PyObject* self = (PyObject*) parser->data;
    return with_args(self, get_cb(self, "on_body"), at, length);
}

static int
on_message_complete(http_parser* parser)
{
    PyObject* self = (PyObject*) parser->data;
    return no_args(self, get_cb(self, "on_message_complete"));
}

PyObject*
Parser_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Parser* self = NULL;

    self = (Parser*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    self->parser = (http_parser*) malloc(sizeof(http_parser));
    if(self->parser == NULL) goto error;
    http_parser_init(self->parser);
    self->parser->data = (void*) self;

    self->parser->on_message_begin = on_message_begin;
    self->parser->on_path = on_path;
    self->parser->on_query_string = on_query_string;
    self->parser->on_url = on_url;
    self->parser->on_fragment = on_fragment;
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
    if(self->parser)
    {
        free(self->parser);
    }
}

PyDoc_STRVAR(Parser_parse_requests__doc__,
"Parser.parse_requests(data)\n\n\
Parse HTTP requests from data. Data should be an \
instance of str.");

static PyObject*
Parser_parse_requests(Parser* parser, PyObject* args, PyObject* kwargs)
{
    char* data = NULL;
    int len = 0;
    size_t resp = 0;

    if(!PyArg_ParseTuple(args, "s#", &data, &len)) return NULL;
    resp = http_parse_requests(parser->parser, data, (size_t) len);
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

    if(PyType_Ready(&ParserType) < 0) return;

    m = Py_InitModule3("pyhttpc", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&ParserType);
    PyModule_AddObject(m, "Parser", (PyObject*) &ParserType);

    PyHttpCModule = m;
}
