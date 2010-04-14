
#include "request.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define LOWER(c) ((c) >= 'A' && (c) <= 'Z') ? ((c) | 0x20) : (c)
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

%%{
    machine http_req_parser;

    action mark {
        assert(parser->genbuf->pos == NULL && "won't overwrite a mark.");
        parser->genbuf->pos = fpc;
    }

    action write_method {
        request->method = buffer_to_string(parser->genbuf, fpc);
        if(request->method == NULL) fbreak;
    }

    action write_scheme {
        request->scheme = buffer_to_string(parser->genbuf, fpc);
        if(request->scheme == NULL) fbreak;
    }

    action write_host {
        request->host = buffer_to_string(parser->genbuf, fpc);
        if(request->scheme == NULL) fbreak;
    }

    action mark_port {
        request->port = 0;
    }
    
    action write_port {
        request->port = request->port*10 + ((*fpc)-'0');
    }
    
    action write_path {
        request->path = buffer_to_string(parser->genbuf, fpc);
        if(request->path == NULL) fbreak;
    }

    action write_query {
        request->query = buffer_to_string(parser->genbuf, fpc);
        if(request->query == NULL) fbreak;
    }

    action write_fragment {
        request->fragment = buffer_to_string(parser->genbuf, fpc);
        if(request->fragment == NULL) fbreak;
    }

    action mark_uri {
        assert(parser->uribuf->pos == NULL && "wont overwrite uri mark");
        parser->uribuf->pos = fpc;
    }

    action write_uri {
        request->uri = buffer_to_string(parser->uribuf, fpc);
        if(request->fragment == NULL) fbreak;
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
        request->vsn_minor = request->vsn_minor*10 + ((*fpc)-'0');
    }

    action mark_name {
        assert(request->hdr_name == NULL && "header name already marked");        
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = fpc;
    }
    
    action write_name {
        request->hdr_name = buffer_to_string(parser->genbuf, fpc);
        if(request->hdr_name == NULL) fbreak;
    }
    
    action mark_value {
        assert(request->hdr_name != NULL && "value must have a name");
        assert(parser->genbuf->pos == NULL && "wont overwrite a mark");
        parser->genbuf->pos = fpc;
    }
    
    action write_value {
        if(!append_header(parser, request, fpc)) fbreak;
    }

    action done {
        build_version(request);
        fbreak;
    }

    include http "http.rl";
    
    main := request;
}%%

%% write data;

int
append_header(RequestParser* parser, Request* request, const char* ptr)
{
    PyObject* tuple = NULL;
    PyObject* val = NULL;

    assert(request->hdr_name != NULL && "invalid internal state: no hdr_name");
    assert(request->headers != NULL && "invalid internal state: no headers");

    tuple = PyTuple_New(2);
    if(tuple == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    
    val = buffer_to_string(parser->genbuf, ptr);
    if(val == NULL) goto error;

    PyTuple_SET_ITEM(tuple, 0, request->hdr_name);
    PyTuple_SET_ITEM(tuple, 1, val);
    
    request->hdr_name = NULL;
    val = NULL;
    
    if(!PyList_Append(request->headers, tuple)) goto error;
    
    return 1;

error:
    Py_XDECREF(tuple);
    Py_XDECREF(val);
    return 0;
}

int
build_version(Request* request)
{
    PyObject* major = NULL;
    PyObject* minor = NULL;
    PyObject* tuple = NULL;
    
    major = PyInt_FromLong(request->vsn_major);
    if(major == NULL) goto error;
    
    minor = PyInt_FromLong(request->vsn_minor);
    if(minor == NULL) goto error;
    
    tuple = PyTuple_New(2);
    if(tuple == NULL) goto error;
    
    PyTuple_SET_ITEM(tuple, 0, major);
    PyTuple_SET_ITEM(tuple, 1, minor);
    
    // SET_ITEM steals
    major = NULL;
    minor = NULL;
    
    request->version = tuple;
    
    return 1;
    
error:
    Py_XDECREF(major);
    Py_XDECREF(minor);
    Py_XDECREF(tuple);
    return 0;
}

int
fill_buffer(RequestParser* p)
{
    const char* genpos = p->genbuf->pos;
    const char* uripos = p->uribuf->pos;
    // Buffer still has data.
    assert(p->bufpos >= p->buffer && "corrupt buffer info");
    assert(p->bufpos <= p->buffer + p->buflen && "corrupt buffer info");

    if(p->bufpos < p->buffer + p->buflen)
    {
        return 1;
    }

    if(genpos) save_buffer(p->genbuf, p->buffer + p->buflen);
    if(uripos) save_buffer(p->uribuf, p->buffer + p->buflen);

    // Drop reference to last data chunk
    Py_XDECREF(p->current);
    p->buffer = NULL;
    p->bufpos = NULL;
    p->buflen = 0;

    p->current = PyIter_Next(p->source);
    if(PyErr_Occurred())
    {
        return -1;
    }
    else if(p->current == NULL)
    {
        return 0;
    }
    
    if(PyString_AsStringAndSize(p->current, &(p->buffer), &(p->buflen)) < 0)
    {
        return -1;
    }
    
    //fprintf(stderr, "NEW BUF: %s\n", p->buffer);
    
    p->bufpos = p->buffer;
    
    // Save current parse buffers and point to 
    // the new data chunk.
    if(genpos) reinit_buffer(p->genbuf, p->buffer);
    if(uripos) reinit_buffer(p->uribuf, p->buffer);
    
    return 1;
}

int
lower_case_match(PyObject* left, const char* lowercase)
{
    char* buf = NULL;
    size_t lclen = 0;
    Py_ssize_t len = 0;
    Py_ssize_t idx = 0;
    
    if(!PyString_AsStringAndSize(left, &buf, &len))
    {
        return -1;
    }
    
    lclen = strlen(lowercase);
    
    // Len could have trailing whitespace so this length
    // is asymetrical.
    if(len < lclen) return 0;

    for(idx = 0; idx < lclen; idx++)
    {
        if(LOWER(buf[idx]) != lowercase[idx]) return 0;
    }
    
    for(; idx < len; idx++)
    {
        if(!IS_SPACE(buf[idx])) return 0;
    }
    
    return 1;
}

PyObject*
find_header(PyObject* headers, const char* lowercasehdr)
{
    PyObject* tpl = NULL;
    Py_ssize_t len = 0;
    Py_ssize_t idx = 0;
    int status;

    if(!PyList_Check(headers))
    {
        PyErr_SetString(PyExc_TypeError, "headers must be a list.");
        return NULL;
    }
    
    len = PyList_Size(headers);
    
    for(idx = 0; idx < len; idx++)
    {
        tpl = PyList_GET_ITEM(headers, idx);
        if(!PyTuple_Check(tpl))
        {
            PyErr_SetString(PyExc_TypeError, "Headers must be tuples");
            return NULL;
        }
        if(PyTuple_GET_SIZE(tpl) != 2)
        {
            PyErr_SetString(PyExc_TypeError, "Headers must be two-tuples.");
            return NULL;
        }
        status = lower_case_match(PyTuple_GET_ITEM(tpl, 0), lowercasehdr);
        if(status < 0) return NULL;
        if(status == 1) return PyTuple_GET_ITEM(tpl, 1);
    }

    return NULL;
}

int
is_chunked(Request* request)
{
    PyObject* val = find_header(request->headers, "transfer-encoding");
    if(PyErr_Occurred()) return -1;
    if(val == NULL) return 0;
    return lower_case_match(val, "chunked");
}

int
content_length(Request* request)
{
    PyObject* val = NULL;
    Py_ssize_t len = -1;
    Py_ssize_t idx = -1;
    char* buf = NULL;
    int ret = -1;
    int count = 0;
    
    val = find_header(request->headers, "content-length");
    if(PyErr_Occurred()) return -1;
    if(val == NULL) return 0;
    if(!PyString_AsStringAndSize(val, &buf, &len)) return -1;
    
    for(idx = 0, ret=0; idx < len; idx++)
    {
        if(IS_DIGIT(buf[idx]))
        {
            count += 1;
            ret = ret*10 + (buf[idx] - '0');
        }
        else if(IS_SPACE(buf[idx]))
        {
            break;
        }
        else
        {
            return -1;
        }        
    }
    
    for(; idx < len; idx++)
    {
        if(!IS_SPACE(buf[idx])) return -1;
    }

    if(count > 0) return ret;
    return -1;
}

int
init_body(Request* request)
{
    int status = is_chunked(request);
    if(PyErr_Occurred()) return 0;
    
    if(status == 1)
    {
        request->body_type = bt_chunked;
        return 1;
    }
    
    request->body_len = content_length(request);
    if(PyErr_Occurred()) return 0;
    
    if(request->body_len > 0)
    {
        request->body_type = bt_length;
    }
    else
    {
        request->body_type = bt_eof;
    }
    
    return 1;
}

int
init_request(RequestParser* parser, Request* request)
{
    int status;
    char* p;
    char* pe;
    int cs;
    
    do
    {
        status = fill_buffer(parser);
        if(status != 1) return 0;

        cs = parser->cs;
        p = parser->bufpos;
        pe = parser->buffer + parser->buflen;

        %% write exec;

        parser->cs = cs;
        parser->bufpos = p;
        parser->nread += p - parser->buffer;

    } while(cs != http_req_parser_error && cs < http_req_parser_first_final);

    if(cs == http_req_parser_error)
    {
        if(!PyErr_Occurred())
        {
            // MAKE MOAR BUTTAH
            PyErr_SetString(PyExc_ValueError, "Failed to parse data stream.");
            return 0;
        }
    }
    
    if(!init_body(request)) return 0;

    return 1;
}

//
//  Request Class
//

static PyObject*
Request_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Request* self = NULL;
    RequestParser* parser = NULL;

    if(!PyArg_ParseTuple(args, "O!", &RequestParserType, &parser)) goto error;

    self = (Request*) type->tp_alloc(type, 0);
    if(self == NULL) goto error;

    Py_INCREF(parser);
    self->parser = parser;
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

    self->headers = PyList_New(0);
    if(self->headers == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }

    self->hdr_name = NULL;

    if(!init_request(self->parser, self)) goto error;

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
}

static int
read_chunked_body(RequestParser* parser, buffer_t* data, int limit)
{
    assert(0 && "implement chunked transfers");
}

static int
read_raw_body(RequestParser* parser, buffer_t* data, int limit)
{
    int limiting = limit < 0 ? 0 : 1;
    size_t chunk;
    int status;
    
    if(!limiting) limit = INT_MAX;

    do
    {
        status = fill_buffer(parser);
        if(status < 0) return 0;
        if(status == 0) return 1;
        reinit_buffer(data, parser->bufpos);
        chunk = MIN(parser->buffer + parser->buflen - parser->bufpos, limit);
        save_buffer(data, parser->bufpos + chunk);
        if(limiting) limit -= chunk;
        parser->bufpos += chunk;
    } while(!limiting || limit > 0);
    
    return 1;
}

static PyObject*
Request_read(Request* self, PyObject* args, PyObject* kwargs)
{
    PyObject* ret = NULL;
    int sizehint = -1;
    buffer_t* data = NULL;
    
    if(!PyArg_ParseTuple(args, "|i", &sizehint)) goto done;
    
    // Future configurable on parser.
    data = init_buffer(INT_MAX);
    if(data == NULL) goto done;

    if(self->body_type == bt_chunked)
    {
        if(!read_chunked_body(self->parser, data, sizehint)) goto done;
    }
    else
    {
        if(!read_raw_body(self->parser, data, sizehint)) goto done;
    }
    
    ret = PyString_FromStringAndSize(data->buf, data->used);
    
done:
    if(data != NULL) free_buffer(data);
    return ret;
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
    {"port", T_INT, offsetof(Request, port), READONLY,
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
    {"read", (PyCFunction) Request_read,
        METH_VARARGS, "Read the HTTP request body."},
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
    self->genbuf = init_buffer(128*1024);
    if(self->genbuf == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    self->uribuf = init_buffer(128*1024);
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

