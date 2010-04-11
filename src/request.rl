
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "request.h"

#define DEF_BUF_SIZE 1024
#define MAX_BUF_LEN  128*1024

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

%%{
    machine http_req_parser;

    action mark {
        assert(parser->mark->pos == NULL && "won't overwrite a mark.");
        parser->mark->pos = fpc;
    }

    action write_method {
        add_field(parser, "method", fpc);
    }

    action write_scheme {
        add_field(parser, "scheme", fpc);
    }

    action write_host {
        add_field(parser, "host", fpc);
    }

    action mark_port {
        parser->port = 0;
    }
    
    action write_port {
        parser->port = parser->port*10 + ((*fpc)-'0');
    }
    
    action write_path {
        add_field(parser, "path", fpc);
    }

    action write_query {
        add_field(parser, "query", fpc);
    }

    action write_fragment {
        add_field(parser, "fragment", fpc);
    }

    action mark_uri {
        assert(parser->mark_uri->pos != NULL && "wont overwrite uri mark");
        parser->mark_uri->pos = fpc;
    }

    action write_uri {
        add_uri(parser, fpc);
    }

    action start_major {
        parser->vsn_major = 0;
    }
    
    action write_major {
        parser->vsn_major = parser->vsn_major*10 + ((*fpc)-'0');
    }
    
    action start_minor {
        parser->vsn_minor = 0;
    }
    
    action write_minor {
        parser->vsn_minor = parser->vsn_minor * 10 + ((*fpc)-'0');
    }

    action mark_name {
        assert(parser->hdr_field == NULL && "header name already marked");        
        assert(parser->mark->pos == NULL && "wont overwrite a mark");
        parser->mark->pos = fpc;
    }
    
    action write_name {
        set_header_name(parser, fpc);
    }
    
    action mark_value {
        assert(parser->hdr_name != NULL && "value must have a name");
        assert(parser->mark->pos == NULL && "wont overwrite a mark");
        parser->mark->pos = fpc;
    }
    
    action write_value {
        add_header(parser, fpc);
    }

    action done {
        parser->body = fpc;
        fbreak;
    }

    include http "http.rl";
    
    main := request;
}%%

%% write data;

static inline PyObject*
mark_obj(mark_buf_t* mbuf, const char* end)
{
    PyObject* val = NULL;
    char* tmp = NULL;
    size_t length = 0;

    assert(mbuf != NULL && "unable to read from null buffer.");
    assert(mbuf->pos != NULL && "attempt to read from null mark");
    assert(end != NULL && "attempt to read to null end");
    assert(end >= mbuf->pos && "unable to read backwards marks");
    
    length = end - mbuf->pos;
    length += mbuf->used;

    tmp = (char*) malloc(length * sizeof(char));
    if(tmp == NULL) return NULL;

    // Copy over stored data
    if(mbuf->used > 0)
    {
        memcpy(tmp, mbuf->buf, mbuf->used);
    }

    // Copy new data
    memcpy(tmp+mbuf->used, mbuf->pos, end-mbuf->pos);

    mbuf->pos = NULL;
    if(mbuf->used > 0) mbuf->used = 0;
    
    val = PyString_FromStringAndSize(tmp, length);
    // If val is null, it'll trigger an error further up.

    if(tmp != NULL) free(tmp);
    return val;
}

void
add_field(RequestParser* parser, char* name, const char* ptr)
{
    PyObject* val = mark_obj(parser->mark, ptr);
    assert(val != NULL && "FIXME: mark_obj returned null");
    if(PyDict_SetItemString(parser->fields, name, val) < 0)
    {
        assert(0 && "FIXME: Failed to set field value");
    }
    Py_XDECREF(val);
}

void
add_uri(RequestParser* parser, const char* ptr)
{
    PyObject* val = mark_obj(parser->mark_uri, ptr);
    assert(val != NULL && "FIXME: mark_obj returned null");
    if(!PyDict_SetItemString(parser->fields, "uri", val) < 0)
    {
        assert(0 && "FIXME: Failed to set field value");
    }
    Py_XDECREF(val);
}

void
set_header_name(RequestParser* parser, const char* ptr)
{
    parser->hdr_name = mark_obj(parser->mark, ptr);
    assert(parser->hdr_name != NULL && "FIXME: mark_obj returned null");
}

void
add_header(RequestParser* parser, const char* ptr)
{
    PyObject* tuple = PyTuple_New(2);
    PyObject* val = mark_obj(parser->mark, ptr);

    assert(tuple != NULL && "FIXME: PyTuple_New return null");
    assert(val != NULL && "FIXME: mark_obj returned null");
    assert(parser->hdr_name != NULL && "cant set header without a name");
    assert(parser->headers != NULL && "cant set header with invalid header list");

    PyTuple_SET_ITEM(tuple, 0, parser->hdr_name);
    PyTuple_SET_ITEM(tuple, 1, val);
    
    if(!PyList_Append(parser->headers, tuple)) goto error;
    
    parser->hdr_name = NULL;
    goto success;

error:
    Py_XDECREF(tuple);
success:
    return;
}

void
add_port(RequestParser* parser)
{
    PyObject* port = NULL;
    
    port = PyInt_FromLong(parser->port);
    if(port == NULL) return;
    
    if(PyDict_SetItemString(parser->fields, "port", port) < 0)
    {
        Py_DECREF(port);
    }
    
    return;
}

void
add_version(RequestParser* parser)
{
    PyObject* major = NULL;
    PyObject* minor = NULL;
    PyObject* tuple = NULL;
    
    major = PyInt_FromLong(parser->vsn_major);
    if(major == NULL) goto error;
    
    minor = PyInt_FromLong(parser->vsn_minor);
    if(minor == NULL) goto error;
    
    tuple = PyTuple_New(2);
    if(tuple == NULL) goto error;
    
    PyTuple_SET_ITEM(tuple, 0, major);
    PyTuple_SET_ITEM(tuple, 1, minor);
    
    // SET_ITEM steals
    major = NULL;
    minor = NULL;
    
    if(PyDict_SetItemString(parser->fields, "version", tuple) < 0)
    {
        goto error;
    }
    
    return;
    
error:
    Py_XDECREF(major);
    Py_XDECREF(minor);
    Py_XDECREF(tuple);
}

void
add_headers(RequestParser* parser)
{
    assert(parser->fields != NULL && "parser fields disappeared");
    assert(parser->headers != NULL && "parser headers disappeared");

    if(PyDict_SetItemString(parser->fields, "headers", parser->headers) < 0)
    {
        return;
    }
    
    Py_DECREF(parser->headers);
    parser->headers = NULL;
}

void
save_mark_buf(mark_buf_t* buf, const char* end)
{
    size_t reqlen = 0;
    size_t newlen = 0;
    char* newbuf = NULL;

    assert(buf->mark != NULL && "unable to save without mark set");
    assert(end != NULL && "unable to save without end");
    assert(end > buf->mark && "unable to save negative memory region");
    assert(buf->len > 0 && "invalid mark_buf length");

    reqlen = end - buf->pos;
    newlen = buf->len;
    while(reqlen > newlen - buf->used) newlen *= 2;

    if(newlen > MAX_BUF_LEN) assert(0 && "FIXME: proper error handling");
    
    newbuf = (char*) malloc(newlen * sizeof(char));
    assert(newbuf != NULL && "FIXME: proper error handling");

    memcpy(newbuf, buf->buf, buf->used);
    memcpy(newbuf+buf->used, buf->pos, reqlen);

    free(buf->buf);
    buf->pos = NULL+1;
    buf->buf = newbuf;
    buf->len = newlen;
    buf->used += reqlen;
}

mark_buf_t*
init_mark_buf()
{
    mark_buf_t* buf = (mark_buf_t*) malloc(sizeof(mark_buf_t));
    if(buf == NULL) return NULL;
    buf->pos = NULL;
    buf->buf = (char*) malloc(DEF_BUF_SIZE * sizeof(char));
    buf->len = DEF_BUF_SIZE;
    buf->used = 0;
    return buf;
}

void
reinit_mark_buf(mark_buf_t* buf, const char* start)
{
    assert(buf != NULL && "mark_buf disappeared");
    
    if(buf->pos == NULL+1)
    {
        buf->pos = start;
    }
    else
    {
        assert(buf->used == 0 && "Saved mark means buf should be empty.");
    }
}

PyObject*
init_field_defaults()
{
    PyObject* fields = PyDict_New();
    
    if(fields == NULL) goto error;

    if(PyDict_SetItemString(fields, "fragment", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "headers", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "host", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "method", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "path", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "port", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "query", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "scheme", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "uri", Py_None) < 0) goto error;
    if(PyDict_SetItemString(fields, "version", Py_None) < 0) goto error;
    
    return fields;

error:
    Py_XDECREF(fields);
    return NULL;
}

int
init_req_parser(RequestParser* parser)
{
    int cs = 0;
    %% write init;

    parser->cs = cs;
    parser->error = 0;

    parser->body = NULL;
    parser->nread = 0;
    
    parser->fields = init_field_defaults();
    if(parser->fields == NULL) goto error;

    parser->port = 80;    
    parser->vsn_major = 0;
    parser->vsn_minor = 0;

    parser->headers = PyList_New(0);
    if(parser->headers == NULL) goto error;

    parser->hdr_name = NULL;

    parser->mark = init_mark_buf();
    if(parser->mark == NULL) goto error;
    
    parser->mark_uri = init_mark_buf();
    if(parser->mark_uri == NULL) goto error;

    return 1;

error:
    Py_XDECREF(parser->fields);
    Py_XDECREF(parser->headers);
    return 0;
}

void
free_mark(mark_buf_t* buf)
{
    if(buf == NULL) return;
    if(buf->buf != NULL) free(buf->buf);
    free(buf);
}

void
free_req_parser(RequestParser* parser)
{
    Py_XDECREF(parser->hdr_name);
    Py_XDECREF(parser->headers);
    Py_XDECREF(parser->fields);

    free_mark(parser->mark);
    free_mark(parser->mark_uri);
}

size_t
exec_req_parser(RequestParser* parser, const char* buffer, size_t len)
{
    const char* p;
    const char* pe;
    int cs = parser->cs;
    
    p = buffer;
    pe = buffer + len;
    
    reinit_mark_buf(parser->mark, p);
    reinit_mark_buf(parser->mark_uri, p);
    
    while(p < pe && cs != http_req_parser_error)
    {
        %% write exec;
    }

    parser->cs = cs;
    parser->nread += p - buffer;
    
    assert(p < pe && "parser boundary error");

    if(parser->body != NULL)
    {
        assert(parser->mark->pos == NULL && "finished parsing with a mark");
        assert(parser->mark_uri->pos == NULL && "finished parsing with a uri");
        assert(parser->hdr_name == NULL && "finished parsing with a hdr name");
        
        add_port(parser);
        add_version(parser);
        add_headers(parser);
    }
    else if(parser->body == NULL && parser->cs == http_req_parser_error)
    {
        return 0;
    }

    if(parser->mark->pos != NULL) save_mark_buf(parser->mark, p);
    if(parser->mark_uri->pos != NULL) save_mark_buf(parser->mark_uri, p);
    
    return 1;
}
