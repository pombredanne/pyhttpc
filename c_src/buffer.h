#ifndef PYHTTPC_BUFFER_H
#define PYHTTPC_BUFFER_H

#define Py_SSIZE_T_CLEAN
#include <Python.h>

#define DEF_BUF_SIZE 1024
#define MAX_BUF_LEN  128*1024

typedef struct {
    const char* pos;
    char*       buf;
    size_t      len;
    size_t      used;
} buffer_t;

buffer_t* init_buffer(void);
void reinit_buffer(buffer_t* buf, const char* start);
void free_buffer(buffer_t* buf);
void save_buffer(buffer_t* buf, const char* end);
PyObject* buffer_to_string(buffer_t* mbuf, const char* end);

#endif // Included buffer.h