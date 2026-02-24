/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_datetime_h
#define python_datetime_h

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "uICAL/datetime.h"

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::DateTime_ptr dt;
    } DateTimeObject;

    extern PyTypeObject DateTimeType;

    // Return a shared_ptr to DateTime or set a Python TypeError and return nullptr.
    uICAL::DateTime_ptr DateTime_from_py(PyObject* obj, const char* where);

    bool PyInit_DateTime(PyObject* module);
}

#endif
