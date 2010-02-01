
#include <Python.h>
#include <assert.h>

#include "state.h"

ParserState*
state_init(ushort type, int fd)
{
    ParserState* state = NULL;
    
    assert(type == HTTP_REQUEST_PARSER || type == HTTP_RESPONSE_PARSER);
    assert(fd >= 0);
    
    state = (ParserState*) malloc(sizeof(ParserState));
    if(state == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }

    state->parser = http_init_parser(type);
    if(state->parser == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    
    state->headers = PyList_New(0);
    if(state->headers == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    
    state->fd = fd;
    state->done = 0;
    state->buffer = NULL;
    state->length = 0;
    state->used = 0;
    state->field = NULL;

    return state;

error:
    if(state != NULL)
    {
        if(state->parser != NULL) free(state->parser);
        if(state->headers != NULL) free(state->headers);
        free(state);
    }
    return NULL;
}

void
state_free(void* st)
{
    ParserState* state = (ParserState*) st;
    if(state == NULL) return;
    
    if(state->buffer != NULL) free(state->buffer);
    if(state->parser != NULL) free(state->parser);

    Py_XDECREF(state->field);
    Py_XDECREF(state->headers);
}

int
state_fill_buffer(ParserState* state)
{
    const uchar* buf = NULL;
    uchar* tmp = NULL;
    size_t len = 0;
    size_t req_len = 0;
    ssize_t status = -1;

    // We have to save what's left in the buffer
    // in the likely case that we haven't ended
    // a parsing at exactly the last byte in the
    // buffer.
    
    len = http_get_buffer(state->parser, &buf);
    if(len < state->length - state->used)
    {
        memcpy(state->buffer + state->used, buf, len);
        state->used += len;
    }
    else
    {
        req_len = state->length + 4096;
        while(state->used + len > req_len) req_len += 4096;
        if(realloc(state->buffer, req_len * sizeof(uchar)) == NULL)
        {
            PyErr_NoMemory();
            goto error;
        }
    }

    // Now fill a new buffer for the parser.
    tmp = (uchar*) malloc(4096 * sizeof(uchar));
    if(tmp == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }

    status = read(state->fd, tmp, 4096);
    if(status < 0)
    {
        PyErr_SetFromErrno(PyExc_IOError);
        free(tmp);
        goto error;
    }
    
    http_set_buffer(state->parser, tmp, (size_t) status);

    return (int) status;

error:
    return -1;
}

PyObject*
state_make_value(ParserState* state)
{
    PyObject* ret = NULL;
    uchar* comp_buf = NULL;
    size_t comp_len = -1;

    if(state->buffer != NULL)
    {
        comp_len = state->used + state->parser->length;
        comp_buf = (uchar*) malloc(comp_len * sizeof(uchar));
        if(comp_buf == NULL)
        {
            PyErr_NoMemory();
            goto error;
        }
        memcpy(comp_buf, state->buffer, state->used);
        memcpy(comp_buf, state->parser->data, state->parser->length);
        state->used = 0;
    }
    else
    {
        comp_buf = (uchar*) state->parser->data;
        comp_len = state->parser->length;
    }
    
    ret = PyString_FromStringAndSize((char*) comp_buf, comp_len);
    if(ret == NULL) goto error;

    goto success;

error:
    Py_XDECREF(ret);
    ret = NULL;
success:
    if(comp_buf != state->parser->data) free(comp_buf);
    return ret;
}

int
state_add_header(ParserState* state)
{
    PyObject* hdrfld = NULL;
    PyObject* hdrval = NULL;
    PyObject* tpl = NULL;
    int ret = -1;

    if(state->field == NULL) goto done;
    hdrfld = state->field;
    state->field = NULL;
    
    hdrval = state_make_value(state);
    if(hdrval == NULL) goto done;
        
    tpl = Py_BuildValue("(OO)", hdrfld, hdrval);
    if(tpl == NULL) goto done;
    
    if(PyList_Append(state->headers, tpl) < 0) goto done;

    ret = 0;

done:
    Py_XDECREF(hdrfld);
    Py_XDECREF(hdrval);
    Py_XDECREF(tpl);
    return ret;
}

PyObject*
state_get_headers(ParserState* state)
{
    PyObject* ret = state->headers;
    PyObject* tmp = PyList_New(0);
    if(tmp == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    state->headers = tmp;
    return ret;
}

PyObject*
state_read(ParserState* state)
{
    unsigned short rval;
    PyObject* ret = NULL;
    PyObject* val = NULL;
    int status;

    ret = PyString_FromString("");
    if(state->done) return ret;
    if(state->parser == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "No parser!\n");
        goto error;
    }
    
    while(1)
    {
        rval = http_run_parser(state->parser);
        //fprintf(stderr, "%u\n", rval);

        switch(rval)
        {
            case HTTP_ERROR:
                PyErr_SetString(PyExc_RuntimeError, "Failed to parse data.");
                goto error;

            case HTTP_CONTINUE:
                if(state_fill_buffer(state) < 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "Failed to fill buffer");
                    goto error;
                }
                break;

            case HTTP_DONE:
                state->done = 1;
                // Fall through
            case HTTP_BODY_EOF:
                status = state_fill_buffer(state);
                if(status > 0)
                {
                    state->parser->data = state->parser->buf;
                    state->parser->length = (size_t) status;
                }
                else
                {
                    state->done = 1;
                }
            case HTTP_BODY:
                val = state_make_value(state);
                if(val == NULL)
                {
                    PyErr_SetString(PyExc_RuntimeError, "Failed to make value\n");
                    goto error;
                }
                PyString_ConcatAndDel(&ret, val);
                if(ret == NULL)
                {
                    PyErr_SetString(PyExc_RuntimeError, "Failed to concat.");
                    goto error;
                }
                if(state->done) goto success;
                break;
            
            default:
                fprintf(stderr, "State: %u\n", rval);
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
