/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#define python_error_c
#include "error.h"

namespace uical_python {
    PyObject* UicalError;

    bool PyInit_Error(PyObject* module) {
        UicalError = PyErr_NewException("uical.error", NULL, NULL);

        if (PyModule_AddObject(module, "error", UicalError) < 0) {
            Py_XDECREF(UicalError);
            Py_CLEAR(UicalError);
            Py_DECREF(module);
            return false;
        }
        return true;
    }
}
