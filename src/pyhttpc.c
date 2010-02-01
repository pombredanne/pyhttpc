
#include <Python.h>
#include "structmember.h"

#include "parser.h"
#include "state.h"

#define WHERE fprintf(stderr, "%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__)

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    
    PyObject* state_ref;
    ParserState* state;

    PyObject* method;
    PyObject* version;
    PyObject* scheme;
    PyObject* host;
    PyObject* port;
    PyObject* path;
    PyObject* query_string;
    PyObject* fragment;
    PyObject* headers;
} Request;

static PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Request* self = NULL;
    PyObject* state_ref = NULL;
    unsigned short rval;
    int status = -1;
    
    if(!PyArg_ParseTuple(args, "O", &state_ref)) goto error;
    
    self = (Request*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;
    
    self->state_ref = state_ref;
    Py_INCREF(self->state_ref);
    self->state = PyCObject_AsVoidPtr(self->state_ref);
    if(self->state == NULL) goto error;
    
    self->method = NULL;
    self->version = NULL;
    self->scheme = NULL;
    self->host = NULL;
    self->port = NULL;
    self->path = NULL;
    self->query_string = NULL;
    self->fragment = NULL;
    self->headers = NULL;
    
    status = state_fill_buffer(self->state);
    if(status < 0) goto error;
    if(status == 0)
    {
        PyErr_SetNone(PyExc_StopIteration);
        goto error;
    }

    while(1)
    {
        rval = http_run_parser(self->state->parser);
        //fprintf(stderr, "STATE: %u\n", rval);

        switch(rval)
        {
            case HTTP_ERROR:
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse data.");
                goto error;

            case HTTP_CONTINUE:
                if(state_fill_buffer(self->state) <= 0) goto error;
                break;
            
            case HTTP_METHOD:
                self->method = state_make_value(self->state);
                if(self->method == NULL) goto error;
                break;

            case HTTP_SCHEME:
                self->scheme = state_make_value(self->state);
                if(self->scheme == NULL) goto error;
                break;

            case HTTP_HOST:
                self->host = state_make_value(self->state);
                if(self->host == NULL) goto error;
                break;
                
            case HTTP_PORT:
                self->port = state_make_value(self->state);
                if(self->port == NULL) goto error;
                break;

            case HTTP_PATH:
                self->path = state_make_value(self->state);
                if(self->path == NULL) goto error;
                break;
            
            case HTTP_QS:
                self->query_string = state_make_value(self->state);
                if(self->query_string == NULL) goto error;
                break;
            
            case HTTP_FRAGMENT:
                self->fragment = state_make_value(self->state);
                if(self->fragment == NULL) goto error;
                break;
            
            case HTTP_HEADER_FIELD:
                if(self->state->field != NULL)
                {
                    PyErr_SetString(PyExc_RuntimeError, "HEADER STATE ERROR");
                    goto error;
                }
                self->state->field = state_make_value(self->state);;
                if(self->state->field == NULL) goto error;
                break;
            
            case HTTP_HEADER_VALUE:
                if(state_add_header(self->state) < 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "Failed adding header.");
                    goto error;
                }
                break;

            case HTTP_DONE:
                self->state->done = 1;
                // Fall through!
            case HTTP_BODY_EOF:
                // Fall through
            case HTTP_HEADERS_DONE:
                self->headers = state_get_headers(self->state);
                if(self->headers == NULL) goto error;
                self->version = Py_BuildValue("(HH)",
                    self->state->parser->vmajor, self->state->parser->vminor);
                if(self->version == NULL) goto error;
                goto success;
            
            default:
                PyErr_SetString(PyExc_RuntimeError, "Invalid parser state.");
                goto error;
        }
    }

    goto success;

error:
    if(!PyErr_Occurred())
    {
        PyErr_SetString(PyExc_RuntimeError, "UNKNOWN ERROR");
    }
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

static void
Request_dealloc(Request* self)
{
    Py_XDECREF(self->method);
    Py_XDECREF(self->version);
    Py_XDECREF(self->scheme);
    Py_XDECREF(self->host);
    Py_XDECREF(self->port);
    Py_XDECREF(self->path);
    Py_XDECREF(self->query_string);
    Py_XDECREF(self->fragment);
    Py_XDECREF(self->headers);
}

static PyObject*
Request_read(Request* self, PyObject* args, PyObject* kwargs)
{
    return state_read(self->state);
}

static PyMemberDef
Request_members[] = {
    {"method", T_OBJECT, offsetof(Request, method), READONLY,
        "This request's method as a string."},
    {"version", T_OBJECT, offsetof(Request, version), READONLY,
        "This request's version as a two-tuple."},
    {"scheme", T_OBJECT, offsetof(Request, scheme), READONLY,
        "This request's URI scheme as a string."},
    {"host", T_OBJECT, offsetof(Request, host), READONLY,
        "This request's URI host as a string."},
    {"port", T_OBJECT, offsetof(Request, port), READONLY,
        "This request's URI port as a string."},
    {"path", T_OBJECT, offsetof(Request, path), READONLY,
        "This request's URI path as a string."},
    {"query_string", T_OBJECT, offsetof(Request, query_string), READONLY,
        "This request's URI query string as a string."},
    {"fragment", T_OBJECT, offsetof(Request, fragment), READONLY,
        "This request's URI fragment as a string."},
    {"headers", T_OBJECT, offsetof(Request, headers), READONLY,
        "This request's HEADERS as a list of 2-tuples."},
    {NULL}
};

static PyMethodDef
Request_methods[] = {
    {"read", (PyCFunction) Request_read,
        METH_VARARGS, "Read the HTTP request body."},
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

typedef struct {
    PyObject_HEAD
    PyObject* fileobj;
    PyObject* state;
    PyObject* mesg_type;
} MessageIter;

void
MessageIter_dealloc(MessageIter* self)
{
    Py_XDECREF(self->fileobj);
    Py_XDECREF(self->state);
}

PyObject*
MessageIter_GET_ITER(MessageIter* self)
{
    Py_INCREF(self);
    return (PyObject*) self;
}

PyObject*
MessageIter_ITER_NEXT(MessageIter* self)
{
    PyObject* tpl = NULL;
    PyObject* req = NULL;
    
    tpl = Py_BuildValue("(O)", self->state);
    if(tpl == NULL) return NULL;

    req = PyObject_CallObject(self->mesg_type, tpl);

    Py_XDECREF(tpl);
    return req;
}

PyTypeObject MessageIterType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.MessageIter",                      /*tp_name*/
    sizeof(MessageIter),                        /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)MessageIter_dealloc,            /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,  /*tp_flags*/
    "PyHttpC MessageIter",                      /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    (getiterfunc)MessageIter_GET_ITER,		    /*tp_iter*/
    (iternextfunc)MessageIter_ITER_NEXT 		/*tp_iternext*/
};

PyDoc_STRVAR(parse_requests__doc__,
"parse_requests(source) -> iterator\n\n\
Create an iterator that yields instances of pyhttpc.Request\n\
that reflect each message as its parsed from the generator ``source``.");

PyObject*
parse_requests(PyObject* ignored, PyObject* args)
{
    MessageIter* self = NULL;
    PyObject* fileobj = NULL;
    ParserState* state = NULL;
    int fd = -1;

    if(!PyArg_ParseTuple(args, "O", &fileobj)) goto error;
    
    self = PyObject_New(MessageIter, &MessageIterType);
    if(self == NULL) goto error;

    fd = PyObject_AsFileDescriptor(fileobj);
    if(fd < 0)
    {
        PyErr_SetString(PyExc_TypeError, "Source must be have a fileno()");
        goto error;
    }
    self->fileobj = fileobj;
    Py_INCREF(self->fileobj);

    state = state_init(HTTP_REQUEST_PARSER, fd);
    if(state == NULL) goto error;
    
    self->state = PyCObject_FromVoidPtr(state, state_free);
    if(self->state == NULL) goto error;

    self->mesg_type = (PyObject*) &RequestType;

    return (PyObject*) self;

error:
    Py_XDECREF(self);
    return NULL;
}

static PyMethodDef pyhttpc_methods[] = {
    {"parse_requests", parse_requests,
        METH_VARARGS, parse_requests__doc__},
    {NULL}
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initpyhttpc(void)
{
    PyObject* m;

    if(PyType_Ready(&RequestType) < 0) return;
    if(PyType_Ready(&MessageIterType) < 0) return;

    m = Py_InitModule3("pyhttpc", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&RequestType);
    Py_INCREF(&MessageIterType);
    
    PyHttpCModule = m;
}
