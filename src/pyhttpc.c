
#define Py_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "request.h"

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    RequestParser* parser;
} RequestReader;

static PyObject*
RequestReader_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    RequestReader* self = NULL;
        
    self = (RequestReader*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;
    self->parser = NULL;

    self->parser = (RequestParser*) malloc(sizeof(RequestParser));
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
RequestReader_dealloc(RequestReader* self)
{
    free_req_parser(self->parser);
}

static PyObject*
RequestReader_read(RequestReader* self, PyObject* args, PyObject* kwargs)
{
    const char* buf = NULL;
    Py_ssize_t len = 0;
    size_t status;
    PyObject* ret = NULL;
    
    if(!PyArg_ParseTuple(args, "s#", &buf, &len)) goto error;

    status = exec_req_parser(self->parser, buf, (size_t) len);

    ret = self->parser->fields;
    self->parser->fields = NULL;
    return ret;

error:
    return NULL;
}

static PyObject*
RequestReader_body(RequestReader* self, PyObject* args, PyObject* kwargs)
{
    PyObject* ret = NULL;
    
    ret = PyString_FromStringAndSize(self->parser->body, )
    Py_RETURN_NONE;
}

static PyMemberDef
RequestReader_members[] = {
    {NULL}
};

static PyMethodDef
RequestReader_methods[] = {
    {"read", (PyCFunction) RequestReader_read,
        METH_VARARGS, "Read a request from some data."},
    {"body", (PyCFunction) RequestReader_body,
        METH_NOARGS, "Get any data that might be part of the body."},
    {NULL}
};

PyTypeObject RequestReaderType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.RequestReader",                    /*tp_name*/
    sizeof(RequestReader),                      /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)RequestReader_dealloc,          /*tp_dealloc*/
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
    RequestReader_methods,                      /*tp_methods*/
    RequestReader_members,                      /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    RequestReader_new,                          /*tp_new*/
};

static PyMethodDef pyhttpc_methods[] = {
    {NULL}
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initpyhttpc(void)
{
    PyObject* m;

    if(PyType_Ready(&RequestReaderType) < 0) return;

    m = Py_InitModule3("pyhttpc", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&RequestReaderType);
    PyModule_AddObject(m, "RequestReader", (PyObject*) &RequestReaderType);
    
    PyHttpCModule = m;
}