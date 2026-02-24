/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_tzmap_h
#define python_tzmap_h

#define PY_SSIZE_T_CLEAN
#include <Python.h>

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::TZMap_ptr tzmap;
    } TZMapObject;

    extern PyTypeObject TZMapType;

    bool PyInit_TZMap(PyObject* module);
}

#endif
