#ifndef PYHTTPC_H
#define PYHTTPC_H

#define Py_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

extern PyTypeObject RequestType;
extern PyTypeObject RequestParserType;

#endif // Include pyhttpc.h