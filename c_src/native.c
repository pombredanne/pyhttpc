
#define Py_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "request.h"

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    http_req_parser_t* parser;
} RequestParser;

static PyObject*
RequestParser_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    RequestParser* self = NULL;
        
    self = (RequestParser*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;
    self->parser = NULL;

    self->parser = (http_req_parser_t*) malloc(sizeof(http_req_parser_t));
    if(self->parser == NULL) goto error;
    if(!init_req_parser(self->parser))
    {
        free(self->parser);
        self->parser = NULL;
        goto error;
    }

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

static void
RequestParser_dealloc(RequestParser* self)
{
    free_req_parser(self->parser);
}

static PyObject*
RequestParser_read(RequestParser* self, PyObject* args, PyObject* kwargs)
{
    const char* buf = NULL;
    Py_ssize_t len = 0;
    PyObject* ret = NULL;
    int status;
    
    if(!PyArg_ParseTuple(args, "s#", &buf, &len)) goto error;

    status = exec_req_parser(self->parser, buf, (size_t) len);

    if(status == PARSE_OK)
    {
        ret = self->parser->fields;
        self->parser->fields = NULL;
        return ret;
    }
    else if(status == PARSE_MORE)
    {
        Py_RETURN_NONE;
    }
    // Else error

    PyErr_SetString(PyExc_ValueError, "Invalid HTTP request.");

error:
    return NULL;
}

static PyObject*
RequestParser_body(RequestParser* self, PyObject* args, PyObject* kwargs)
{
    Py_RETURN_NONE;
}

static PyMemberDef
RequestParser_members[] = {
    {NULL}
};

static PyMethodDef
RequestParser_methods[] = {
    {"read", (PyCFunction) RequestParser_read,
        METH_VARARGS, "Read a request from some data."},
    {"body", (PyCFunction) RequestParser_body,
        METH_NOARGS, "Get any data that might be part of the body."},
    {NULL}
};

PyTypeObject RequestParserType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.native.RequestParser",             /*tp_name*/
    sizeof(RequestParser),                      /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)RequestParser_dealloc,          /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT,                         /*tp_flags*/
    "PyHttpC Request Reader",                   /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    0,		                                    /*tp_iter*/
    0,		                                    /*tp_iternext*/
    RequestParser_methods,                      /*tp_methods*/
    RequestParser_members,                      /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    RequestParser_new,                          /*tp_new*/
};

static PyMethodDef pyhttpc_methods[] = {
    {NULL}
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initnative(void)
{
    PyObject* m;

    if(PyType_Ready(&RequestParserType) < 0) return;

    m = Py_InitModule3("native", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&RequestParserType);
    PyModule_AddObject(m, "RequestParser", (PyObject*) &RequestParserType);
    
    PyHttpCModule = m;
}