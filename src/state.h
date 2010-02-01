
#ifndef PYHTTP_STATE_H
#define PYHTTP_STATE_H

#include "parser.h"

typedef struct {
    http_parser* parser;
    int fd;
    int done;

    uchar* buffer;
    size_t length;
    size_t used;

    PyObject* field;
    PyObject* headers;
} ParserState;

ParserState* state_init(ushort type, int fd);
void state_free(void* state);

PyObject* state_make_value(ParserState* state);
PyObject* state_get_headers(ParserState* state);
PyObject* state_read(ParserState* state);

int state_fill_buffer(ParserState* state);
int state_add_header(ParserState* state);

#endif // Included state.h