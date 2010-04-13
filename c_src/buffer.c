
#include "buffer.h"

buffer_t*
init_buffer(void)
{
    buffer_t* buf = (buffer_t*) malloc(sizeof(buffer_t));
    if(buf == NULL) return NULL;
    buf->pos = NULL;
    buf->buf = (char*) malloc(DEF_BUF_SIZE * sizeof(char));
    buf->len = DEF_BUF_SIZE;
    buf->used = 0;
    return buf;
}

void
reinit_buffer(buffer_t* buf, const char* start)
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
    assert(buf->mark != NULL && "unable to save without mark set");
    assert(end != NULL && "unable to save without end");
    assert(end > buf->mark && "unable to save negative memory region");
    assert(buf->len > 0 && "invalid mark_buf length");

    reqlen = end - buf->pos;

    if(reqlen < buf->len - buf->used)
    {
        memcpy(buf->buf, buf->pos, reqlen);
        buf->used += reqlen;
        buf->pos = NULL+1;
        return;
    }

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

PyObject*
buffer_to_string(buffer_t* mbuf, const char* end)
{
    PyObject* val = NULL;

    save_buffer(mbuf, end);
    assert(buf->pos == NULL+1 && "save_mark_buf error");

    val = PyString_FromStringAndSize(mbuf->buf, mbuf->used);
    mbuf->pos = NULL;
    mbuf->used = 0;
    
    return val;
}
