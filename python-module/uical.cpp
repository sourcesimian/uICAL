/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datestamp.h"
#include "uICAL/calendar.h"
#include "uICAL/rruleiter.h"

#include "error.h"
#include "calendar.h"
#include "rrule.h"

namespace uical_python {
    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        .m_name = "uICAL",
        .m_doc = "A light weight implementation of the ICAL specification",
        .m_size = -1,
    };
}

PyMODINIT_FUNC
PyInit_uICAL(void)
{
    PyObject *m;

    m = PyModule_Create(&uical_python::module);
    if (m == NULL)
        return NULL;

    if (!uical_python::PyInit_Error(m)) {
        //PY_DECREF(m);  TODO error: use of undeclared identifier 'PY_DECREF'
        return NULL;
    }

    if (!uical_python::PyInit_Calendar(m)) {
        //PY_DECREF(m);  TODO error: use of undeclared identifier 'PY_DECREF'
        return NULL;
    }

    if (!uical_python::PyInit_RRule(m)) {
        //PY_DECREF(m);  TODO error: use of undeclared identifier 'PY_DECREF'
        return NULL;
    }

    return m;
}
