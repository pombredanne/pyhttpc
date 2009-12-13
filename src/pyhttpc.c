
#include <Python.h>
#include "structmember.h"
#include "parser.h"

#define WHERE fprintf(stderr, "[%s]%s:%d\n", __FILE__, __FUNCTION__, __LINE__)

PyObject* PyHttpCModule = NULL;

typedef struct {
    PyObject_HEAD
    PyObject* headers;
    char is_final;

    // CObject around http_parser.
    PyObject* parser;

    // Data source
    PyObject* iter;
    PyObject* handle;
    PyObject* buffer;
    PyObject* field;
} Message;

static int
Message_fill_buffer(Message* self, http_parser* parser)
{
    PyObject* str = NULL;
    const uchar* buf = NULL;
    Py_ssize_t len = 0;
    int ret = -1;

    // We may not have completely drained the
    // previous buffer, store the contents here
    // so we can prepend it when we finish the
    // next parsed entity.
    len = (Py_ssize_t) http_get_buffer(parser, &buf);
    if(len > 0)
    {
        str = PyString_FromStringAndSize((char*) buf, len);
        if(str == NULL) goto error;

        if(self->buffer != NULL)
        {
            PyString_ConcatAndDel(&(self->buffer), str);
            str = NULL;
            if(self->buffer == NULL) goto error;
        }
        else
        {
            self->buffer = str;
            str = NULL;
        }
    }

    if(self->iter == NULL)
    {
        PyErr_SetNone(PyExc_StopIteration);
        goto error;
    }
    
    str = PyIter_Next(self->iter);
    if(str == NULL)
    {
        PyErr_SetNone(PyExc_StopIteration);
        goto error;
    }
    
    // Set the parser buffer.
    if(!PyString_Check(str))
    {
        PyErr_SetString(PyExc_TypeError, "Source iterator yielded non-string.");
        goto error;
    }
    
    if(PyString_AsStringAndSize(str, (char**) &buf, &len) < 0) goto error;
    
    // Store a handle to the string object so it doesn't
    // get deallocated out from under us.
    Py_XDECREF(self->handle);
    self->handle = str;
    
    http_set_buffer(parser, buf, (size_t) len);

    ret = 0;
    goto success;

error:
    Py_XDECREF(str);
success:
    return ret;
}

static PyObject*
Message_mk_value(Message* self, http_parser* parser)
{
    PyObject* ret = NULL;
    char* data = (char*) parser->data;
    Py_ssize_t length = (Py_ssize_t) parser->length;
    
    ret = PyString_FromStringAndSize(data, length);
    if(ret == NULL) goto error;

    // Combine the new value with any previous data
    // we had in a partial buffer.
    if(self->buffer != NULL && length == 0)
    {
        ret = self->buffer;
        self->buffer = NULL;
    }
    else if(self->buffer != NULL)
    {
        PyString_ConcatAndDel(&(self->buffer), ret);
        ret = NULL;
        if(self->buffer == NULL) goto error;
        ret = self->buffer;
        self->buffer = NULL;
    }

    goto success;

error:
    Py_XDECREF(ret);
    ret = NULL;
success:
    return ret;
}

static int
Message_add_header(Message* self, http_parser* parser)
{
    PyObject* hdrfld = NULL;
    PyObject* hdrval = NULL;
    PyObject* tpl = NULL;
    int ret = -1;

    if(self->field == NULL) goto done;
    hdrfld = self->field;
    self->field = NULL;
    
    hdrval = Message_mk_value(self, parser);
    if(hdrval == NULL) goto done;
        
    tpl = Py_BuildValue("(OO)", hdrfld, hdrval);
    if(tpl == NULL) goto done;
    
    if(PyList_Append(self->headers, tpl) < 0) goto done;

    ret = 0;

done:
    Py_XDECREF(hdrfld);
    Py_XDECREF(hdrval);
    Py_XDECREF(tpl);
    return ret;
}

static PyObject*
Message_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Message* self = NULL;
    PyObject* parser = NULL;
    PyObject* iter = NULL;
    
    if(!PyArg_ParseTuple(args, "OO", &parser, &iter)) goto error;
    
    self = (Message*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    Py_INCREF(parser);
    self->parser = parser;
    
    Py_INCREF(iter);
    self->iter = iter;

    self->handle = NULL;
    self->buffer = NULL;
    self->field = NULL;

    self->headers = PyList_New(0);
    if(self->headers == NULL)
    {
        PyErr_NoMemory();
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
Message_dealloc(Message* self)
{
    Py_XDECREF(self->headers);
    Py_XDECREF(self->parser);
    Py_XDECREF(self->iter);
    Py_XDECREF(self->handle);
    Py_XDECREF(self->buffer);
    Py_XDECREF(self->field);
}

static PyObject*
Message_read(Message* self, PyObject* args, PyObject* kwargs)
{
    unsigned short rval;
    http_parser* p = NULL;
    PyObject* ret = PyString_FromString("");
    if(ret == NULL) return NULL;
    PyObject* val = NULL;

    p = PyCObject_AsVoidPtr(self->parser);
    if(p == NULL) goto error;
    
    while(1)
    {
        rval = http_run_parser(p);
        //fprintf(stderr, "%u\n", rval);

        switch(rval)
        {
            case HTTP_ERROR:
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse data.");
                goto error;

            case HTTP_CONTINUE:
                if(Message_fill_buffer((Message*) self, p) < 0) goto error;
                break;

            case HTTP_BODY:
                val = Message_mk_value(self, p);
                if(val == NULL) goto error;
                PyString_ConcatAndDel(&ret, val);
                val = NULL;
                if(ret == NULL) goto error;
                break;
            
            case HTTP_DONE:
                goto success;
            
            default:
                PyErr_SetString(PyExc_RuntimeError, "Invalid parser state.");
                goto error;
        }
    }

error:
    Py_XDECREF(ret);
    ret = NULL;
success:
    return ret;
}

static PyObject*
Message_readline(Message* self, PyObject* args, PyObject* kwargs)
{
    Py_RETURN_NONE;
}

static PyObject*
Message_readlines(Message* self, PyObject* args, PyObject* kwargs)
{
    Py_RETURN_NONE;
}

static PyObject*
Message_ITER(PyObject* self)
{
    Py_INCREF(self);
    return (PyObject*) self;
}

static PyObject*
Message_NEXT(Message* self)
{
    Py_RETURN_NONE;
}

static PyMemberDef
Message_members[] = {
    {"headers", T_OBJECT, offsetof(Message, headers), READONLY,
        "List of two-tuples."},
    {"keep_alive", T_BOOL, offsetof(Message, is_final), READONLY,
        "Is this the last message in the stream?"},
    {NULL}
};

static PyMethodDef
Message_methods[] = {
    {"read", (PyCFunction)Message_read,
        METH_VARARGS, "stuff"},
    {"readline", (PyCFunction)Message_readline,
        METH_VARARGS, "stuff"},
    {"readlines", (PyCFunction)Message_readlines,
        METH_VARARGS, "stuff"},
    {NULL}
};

PyTypeObject MessageType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.Message",                          /*tp_name*/
    sizeof(Message),                            /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)Message_dealloc,                /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT
        | Py_TPFLAGS_BASETYPE
        | Py_TPFLAGS_HAVE_ITER,                 /*tp_flags*/
    "PyHttpC Message",                          /*tp_doc*/
    0,		                                    /*tp_traverse*/
    0,		                                    /*tp_clear*/
    0,		                                    /*tp_richcompare*/
    0,		                                    /*tp_weaklistoffset*/
    (getiterfunc)Message_ITER,		            /*tp_iter*/
    (iternextfunc)Message_NEXT,		            /*tp_iternext*/
    Message_methods,                            /*tp_methods*/
    Message_members,                            /*tp_members*/
    0,                                          /*tp_getset*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    0,                                          /*tp_init*/
    0,                                          /*tp_alloc*/
    Message_new,                                /*tp_new*/
};

typedef struct {
    Message base;
    PyObject* method;
    PyObject* version;
    PyObject* scheme;
    PyObject* host;
    PyObject* port;
    PyObject* path;
    PyObject* query_string;
    PyObject* fragment;
} Request;

static PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    unsigned short rval;
    http_parser* p;
    
    Request* self = (Request*) Message_new(type, args, kwargs);
    if(self == NULL) goto error;

    p = PyCObject_AsVoidPtr(self->base.parser);
    if(p == NULL) goto error;
    
    self->method = NULL;
    self->version = NULL;
    self->scheme = NULL;
    self->host = NULL;
    self->port = NULL;
    self->path = NULL;
    self->query_string = NULL;
    self->path = NULL;
    
    if(Message_fill_buffer((Message*) self, p) < 0) goto error;

    while(1)
    {
        rval = http_run_parser(p);
        //fprintf(stderr, "%u\n", rval);

        switch(rval)
        {
            case HTTP_ERROR:
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse data.");
                goto error;

            case HTTP_CONTINUE:
                if(Message_fill_buffer((Message*) self, p) < 0) goto error;
                break;
            
            case HTTP_METHOD:
                self->method = Message_mk_value((Message*) self, p);
                if(self->method == NULL) goto error;
                break;

            case HTTP_SCHEME:
                self->scheme = Message_mk_value((Message*) self, p);
                if(self->scheme == NULL) goto error;
                break;

            case HTTP_HOST:
                self->host = Message_mk_value((Message*) self, p);
                if(self->host == NULL) goto error;
                break;
                
            case HTTP_PORT:
                self->port = Message_mk_value((Message*) self, p);
                if(self->port == NULL) goto error;
                break;

            case HTTP_PATH:
                self->path = Message_mk_value((Message*) self, p);
                if(self->path == NULL) goto error;
                break;
            
            case HTTP_QS:
                self->query_string = Message_mk_value((Message*) self, p);
                if(self->query_string == NULL) goto error;
                break;
            
            case HTTP_FRAGMENT:
                self->fragment = Message_mk_value((Message*) self, p);
                if(self->fragment == NULL) goto error;
                break;
            
            case HTTP_HEADER_FIELD:
                if(self->base.field != NULL)
                {
                    PyErr_SetString(PyExc_RuntimeError, "HEADER STATE ERROR");
                    goto error;
                }
                self->base.field = Message_mk_value((Message*) self, p);
                if(self->base.field == NULL) goto error;
                break;
            
            case HTTP_HEADER_VALUE:
                if(Message_add_header((Message*) self, p) < 0)
                {
                    fprintf(stderr, "Failed to add header.\n");
                    goto error;
                }
                break;
            
            case HTTP_HEADERS_DONE:
            case HTTP_DONE:
                self->version = Py_BuildValue("(HH)", p->vmajor, p->vminor);
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
    {NULL}
};

static PyMethodDef
Request_methods[] = {
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
    &MessageType,                               /*tp_base*/
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
    PyObject* parser;
    PyObject* iter;
    PyTypeObject* mesg_type;
} MessageIter;

void
MessageIter_dealloc(MessageIter* self)
{
    Py_XDECREF(self->parser);
    Py_XDECREF(self->iter);
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
    
    tpl = Py_BuildValue("OO", self->parser, self->iter);
    if(tpl == NULL) return NULL;

    req = PyObject_CallObject((PyObject*) self->mesg_type, tpl);

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
    PyObject* iter = NULL;
    http_parser* parser = NULL;
    
    if(!PyArg_ParseTuple(args, "O", &iter)) goto error;

    if(!PyIter_Check(iter))
    {
        PyErr_SetString(PyExc_TypeError, "source must be an iterator.");
        goto error;
    }
    
    self = PyObject_New(MessageIter, &MessageIterType);
    if(self == NULL) goto error;

    self->mesg_type = (PyTypeObject*) &RequestType;

    Py_INCREF(iter);
    self->iter = iter;

    parser = http_init_parser(HTTP_REQUEST_PARSER);
    if(parser == NULL) goto error;
    self->parser = PyCObject_FromVoidPtr((void*) parser, free);

    return (PyObject*) self;

error:
    Py_XDECREF(iter);
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

    if(PyType_Ready(&MessageType) < 0) return;
    if(PyType_Ready(&RequestType) < 0) return;
    if(PyType_Ready(&MessageIterType) < 0) return;

    m = Py_InitModule3("pyhttpc", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&MessageType);
    Py_INCREF(&RequestType);
    Py_INCREF(&MessageIterType);
    
    PyHttpCModule = m;
}
