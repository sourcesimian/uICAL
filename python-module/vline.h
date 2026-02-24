/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_vline_h
#define python_vline_h

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "uICAL/vline.h"

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::VLine_ptr vline;
    } VLineObject;

    extern PyTypeObject VLineType;

    bool PyInit_VLine(PyObject* module);
}

#endif
