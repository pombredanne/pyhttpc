
#include "pyhttpc.h"

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyObject* PyHttpCModule = NULL;

static PyMethodDef pyhttpc_methods[] = {
    {NULL}
};

PyMODINIT_FUNC
initnative(void)
{
    PyObject* m;

    if(PyType_Ready(&RequestType) < 0) return;
    if(PyType_Ready(&RequestParserType) < 0) return;

    m = Py_InitModule3("native", pyhttpc_methods, "An HTTP Parser");
    if(m == NULL) return;

    Py_INCREF(&RequestType);
    PyModule_AddObject(m, "Request", (PyObject*) &RequestType);

    Py_INCREF(&RequestParserType);
    PyModule_AddObject(m, "RequestParser", (PyObject*) &RequestParserType);
    
    PyHttpCModule = m;
}