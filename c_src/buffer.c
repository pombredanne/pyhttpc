
#include "buffer.h"

#define WHERE fprintf(stderr, "%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__)

#define DEF_BUF_SIZE 1024

buffer_t*
init_buffer(size_t maxsize)
{
    buffer_t* buf = (buffer_t*) malloc(sizeof(buffer_t));
    if(buf == NULL) return NULL;
    buf->max = maxsize;
    buf->pos = NULL;
    buf->buf = (char*) malloc(DEF_BUF_SIZE * sizeof(char));
    if(buf->buf == NULL)
    {
        free(buf);
        return NULL;
    }
    buf->len = DEF_BUF_SIZE;
    buf->used = 0;
    return buf;
}

void
reinit_buffer(buffer_t* buf, const char* start)
{
    assert(buf != NULL && "mark_buf disappeared");
    assert(buf->pos == NULL && "remaining data has not been saved.");
    buf->pos = start;
}

void
free_buffer(buffer_t* buf)
{
    if(buf == NULL) return;
    if(buf->buf != NULL) free(buf->buf);
    free(buf);
}

void
save_buffer(buffer_t* buf, const char* end)
{
    size_t reqlen = 0;
    size_t newlen = 0;
    char* newbuf = NULL;

    assert(buf != NULL && "unable to read from NULL mark buffer");
    assert(buf->pos != NULL && "unable to save without mark set");
    assert(end != NULL && "unable to save without end");
    assert(end >= buf->pos && "unable to save negative memory region");
    assert(end - buf->pos < (1024*1024*1024) && "corrupted position info");
    assert(buf->len > 0 && "invalid mark_buf length");

    // Marked just before exiting. We don't
    // have anything to save.
    if(end == buf->pos)
    {
        buf->pos = NULL;
        return;
    }

    reqlen = end - buf->pos;

    if(reqlen < buf->len - buf->used)
    {
        memcpy(buf->buf+buf->used, buf->pos, reqlen);
        buf->pos = NULL;
        buf->used += reqlen;
        return;
    }

    newlen = buf->len;
    while(reqlen > newlen - buf->used) newlen *= 2;

    if(newlen > buf->max) assert(0 && "FIXME: proper error handling");
    
    newbuf = (char*) malloc(newlen * sizeof(char));
    assert(newbuf != NULL && "FIXME: proper error handling");

    memcpy(newbuf, buf->buf, buf->used);
    memcpy(newbuf+buf->used, buf->pos, reqlen);

    free(buf->buf);
    buf->pos = NULL;
    buf->buf = newbuf;
    buf->len = newlen;
    buf->used += reqlen;
}

PyObject*
buffer_to_string(buffer_t* buf, const char* end)
{
    PyObject* val = NULL;

    save_buffer(buf, end);
    assert(buf->pos == NULL && "save_mark_buf error");

    val = PyString_FromStringAndSize(buf->buf, buf->used);
    if(val == NULL) PyErr_NoMemory();
    
    buf->pos = NULL;
    buf->used = 0;
    
    return val;
}
