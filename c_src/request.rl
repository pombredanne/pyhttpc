
#include "request.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

%%{
    machine http_req_parser;

    action mark {
        assert(parser->genbuf->pos == NULL && "won't overwrite a mark.");
        parser->genbuf->pos = fpc;
    }

    action write_method {
    }

    action write_scheme {
    }

    action write_host {
    }

    action mark_port {
        request->port = 0;
    }
    
    action write_port {
        request->port = request->port*10 + ((*fpc)-'0');
    }
    
    action write_path {
    }

    action write_query {
    }

    action write_fragment {
    }

    action mark_uri {
        assert(parser->uribuf->pos != NULL && "wont overwrite uri mark");
        parser->uribuf->pos = fpc;
    }

    action write_uri {
    }

    action start_major {
        request->vsn_major = 0;
    }
    
    action write_major {
        request->vsn_major = request->vsn_major*10 + ((*fpc)-'0');
    }
    
    action start_minor {
        request->vsn_minor = 0;
    }
    
    action write_minor {
        request->vsn_minor = request->vsn_minor * 10 + ((*fpc)-'0');
    }

    action mark_name {
        assert(request->hdr_field == NULL && "header name already marked");        
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = fpc;
    }
    
    action write_name {
    }
    
    action mark_value {
        assert(request->hdr_name != NULL && "value must have a name");
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = fpc;
    }
    
    action write_value {
    }

    action done {
        parser->body = fpc;
        fbreak;
    }

    include http "http.rl";
    
    main := request;
}%%

%% write data;

// void
// add_field(http_req_parser_t* parser, char* name, const char* ptr)
// {
//     PyObject* val = mark_obj(parser->mark, ptr);
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     if(PyDict_SetItemString(parser->fields, name, val) < 0)
//     {
//         assert(0 && "FIXME: Failed to set field value");
//     }
//     Py_XDECREF(val);
// }

// void
// add_uri(http_req_parser_t* parser, const char* ptr)
// {
//     PyObject* val = mark_obj(parser->mark_uri, ptr);
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     if(!PyDict_SetItemString(parser->fields, "uri", val) < 0)
//     {
//         assert(0 && "FIXME: Failed to set field value");
//     }
//     Py_XDECREF(val);
// }

// void
// set_header_name(http_req_parser_t* parser, const char* ptr)
// {
//     parser->hdr_name = mark_obj(parser->mark, ptr);
//     assert(parser->hdr_name != NULL && "FIXME: mark_obj returned null");
// }

// void
// add_header(http_req_parser_t* parser, const char* ptr)
// {
//     PyObject* tuple = PyTuple_New(2);
//     PyObject* val = mark_obj(parser->mark, ptr);
// 
//     assert(tuple != NULL && "FIXME: PyTuple_New return null");
//     assert(val != NULL && "FIXME: mark_obj returned null");
//     assert(parser->hdr_name != NULL && "cant set header without a name");
//     assert(parser->headers != NULL && "cant set header without header list");
// 
//     PyTuple_SET_ITEM(tuple, 0, parser->hdr_name);
//     PyTuple_SET_ITEM(tuple, 1, val);
//     
//     if(!PyList_Append(parser->headers, tuple)) goto error;
//     
//     parser->hdr_name = NULL;
//     goto success;
// 
// error:
//     Py_XDECREF(tuple);
// success:
//     return;
// }

// void
// add_version(http_req_parser_t* parser)
// {
//     PyObject* major = NULL;
//     PyObject* minor = NULL;
//     PyObject* tuple = NULL;
//     
//     major = PyInt_FromLong(parser->vsn_major);
//     if(major == NULL) goto error;
//     
//     minor = PyInt_FromLong(parser->vsn_minor);
//     if(minor == NULL) goto error;
//     
//     tuple = PyTuple_New(2);
//     if(tuple == NULL) goto error;
//     
//     PyTuple_SET_ITEM(tuple, 0, major);
//     PyTuple_SET_ITEM(tuple, 1, minor);
//     
//     // SET_ITEM steals
//     major = NULL;
//     minor = NULL;
//     
//     if(PyDict_SetItemString(parser->fields, "version", tuple) < 0)
//     {
//         goto error;
//     }
//     
//     return;
//     
// error:
//     Py_XDECREF(major);
//     Py_XDECREF(minor);
//     Py_XDECREF(tuple);
// }

// void
// add_headers(http_req_parser_t* parser)
// {
//     assert(parser->fields != NULL && "parser fields disappeared");
//     assert(parser->headers != NULL && "parser headers disappeared");
// 
//     if(PyDict_SetItemString(parser->fields, "headers", parser->headers) < 0)
//     {
//         return;
//     }
//     
//     Py_DECREF(parser->headers);
//     parser->headers = NULL;
// }

int
fill_buffer(RequestParser* p)
{
    // Buffer still has data.
    assert(p->bufpos >= p->buffer && "corrupt buffer info");
    assert(p->bufpos <= p->buffer + p->buflen && "corrupt buffer info");

    if(p->bufpos < p->bufend)
    {
        return 1;
    }

    // Drop reference to last data chunk
    Py_XDECREF(p->current);
    p->buffer = NULL;
    p->bufpos = NULL;
    p->buflen = 0;

    p->current = PyIter_Next(p->source);
    if(p->current == NULL && PyErr_Occurred())
    {
        return -1;
    }
    else if(p->current == NULL)
    {
        return 0;
    }
    
    if(PyString_AsStringAndSize(p->current, &(p->buffer), (&p->buflen)) < 0)
    {
        return -1;
    }
    
    // Save current parse buffers and point to 
    // the new data chunk.
    bounce_buffer(parser->genbuf, p->buffer);
    bounce_buffer(parser->uribuf, p->buffer);
    
    return 1;
}

int
init_request(RequestParser* parser, Request* request)
{
    int status;
    const char* p;
    const char* pe;
    int cs = parser->cs;
    
    status = fill_buffer(parser);
    if(status != 1) return status;
    
    p = parser->bufpos;
    pe = parser->buffer + parser->buflen;
    
    %% write exec;

    parser->cs = cs;
    parser->nread += p - parser->buffer;

/// HERE

    assert(p < pe && "parser boundary error");

    if(parser->body != NULL)
    {
        assert(parser->mark->pos == NULL && "finished parsing with a mark");
        assert(parser->mark_uri->pos == NULL && "finished parsing with a uri");
        assert(parser->hdr_name == NULL && "finished parsing with a hdr name");
    }
    else if(parser->body == NULL && parser->cs == http_req_parser_error)
    {
        return 0;
    }
    
    return 1;
}

//
//  Request Class
//

static PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Request* self = NULL;
    //PyObject* parser = NULL;

    self = (Request*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    self->parser = NULL;
    self->method = NULL;
    self->uri = NULL;
    self->scheme = NULL;
    self->host = NULL;
    self->port = 80;
    self->path = NULL;
    self->query = NULL;
    self->fragment = NULL;
    self->version = NULL;
    self->vsn_major = 0;
    self->vsn_minor = 0;
    self->headers = NULL;
    self->hdr_name = NULL;
    self->body = NULL;

    goto success;

error:
    Py_XDECREF(self);
    self = NULL;

success:
    return (PyObject*) self;
}

static void
Request_dealloc(Request* self)
{
    Py_XDECREF(self->parser);
    Py_XDECREF(self->method);
    Py_XDECREF(self->uri);
    Py_XDECREF(self->scheme);
    Py_XDECREF(self->host);
    Py_XDECREF(self->path);
    Py_XDECREF(self->query);
    Py_XDECREF(self->fragment);
    Py_XDECREF(self->version);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->hdr_name);
    Py_XDECREF(self->body);
}

static PyMemberDef
Request_members[] = {
    {"method", T_OBJECT, offsetof(Request, method), READONLY,
        "This request's method as a string."},
    {"uri", T_OBJECT, offsetof(Request, uri), READONLY,
        "This request's original URI."},
    {"scheme", T_OBJECT, offsetof(Request, scheme), READONLY,
        "This request's URI scheme as a string."},
    {"host", T_OBJECT, offsetof(Request, host), READONLY,
        "This request's URI host as a string."},
    {"port", T_OBJECT, offsetof(Request, port), READONLY,
        "This request's URI port as a string."},
    {"path", T_OBJECT, offsetof(Request, path), READONLY,
        "This request's URI path as a string."},
    {"query", T_OBJECT, offsetof(Request, query), READONLY,
        "This request's URI query string as a string."},
    {"fragment", T_OBJECT, offsetof(Request, fragment), READONLY,
        "This request's URI fragment as a string."},
    {"version", T_OBJECT, offsetof(Request, version), READONLY,
        "This request's version as a two-tuple."},
    {"headers", T_OBJECT, offsetof(Request, headers), READONLY,
        "This request's headers as a list of two-tuples."},
    {NULL}
};

static PyMethodDef
Request_methods[] = {
    {NULL}
};

PyTypeObject RequestType = {
    PyObject_HEAD_INIT(NULL)
    0,                                          /*ob_size*/
    "pyhttpc.native.Request",                   /*tp_name*/
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
    Py_TPFLAGS_DEFAULT,                         /*tp_flags*/
    "PyHttpC Request",                          /*tp_doc*/
    0,                                          /*tp_traverse*/
    0,                                          /*tp_clear*/
    0,                                          /*tp_richcompare*/
    0,                                          /*tp_weaklistoffset*/
    0,                                          /*tp_iter*/
    0,                                          /*tp_iternext*/
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

//
// RequestParser Class
//

static PyObject*
RequestParser_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    RequestParser* self = NULL;
    PyObject* src = NULL;
    int cs = 0;
    
    self = (RequestParser*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    if(!PyArg_ParseTuple(args, "O", &src)) goto error;
    if(!PyIter_Check(src))
    {
        PyErr_SetString(PyExc_TypeError, "Data source must be an iterator.");
        goto error;
    }
    self->source = PyObject_GetIter(src);
    if(self->source == NULL)
    {
        if(!PyErr_Occurred())
        {
            PyErr_SetString(PyExc_TypeError, "Unable create source iterator.");
        }
        goto error;
    }

    self->cs = 0;
    self->current = NULL;
    self->bufpos = NULL;
    self->buffer = NULL;
    self->buflen = 0;
    self->nread = 0;
    self->request = NULL;
    self->genbuf = init_buffer();
    if(self->genbuf == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    self->uribuf = init_buffer();
    if(self->uribuf == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }

    %% write init;
    self->cs = cs;

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
    Py_XDECREF(self->source);
    Py_XDECREF(self->current);
    Py_XDECREF(self->request);
    free_buffer(self->genbuf);
    free_buffer(self->uribuf);
}

PyObject*
RequestParser_GET_ITER(RequestParser* self)
{
    Py_INCREF(self);
    return (PyObject*) self;
}

PyObject*
RequestParser_ITER_NEXT(RequestParser* self)
{
    PyObject* tpl = NULL;
    PyObject* req = NULL;
    
    tpl = Py_BuildValue("(O)", self);
    if(tpl == NULL) return NULL;

    req = PyObject_CallObject((PyObject*) &RequestType, tpl);

    Py_XDECREF(tpl);
    return req;
}

static PyMemberDef
RequestParser_members[] = {
    {NULL}
};

static PyMethodDef
RequestParser_methods[] = {
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,  /*tp_flags*/
    "PyHttpC Request Parser",                   /*tp_doc*/
    0,                                          /*tp_traverse*/
    0,                                          /*tp_clear*/
    0,                                          /*tp_richcompare*/
    0,                                          /*tp_weaklistoffset*/
    (getiterfunc)RequestParser_GET_ITER,        /*tp_iter*/
    (iternextfunc)RequestParser_ITER_NEXT,      /*tp_iternext*/
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

