/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/tzmap.h"
#include "uICAL/tz.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "datetime.h"
#include "tzmap.h"

namespace uical_python {

    static void
    DateTime_dealloc(DateTimeObject *self)
    {
        self->dt = nullptr;
        Py_TYPE(self)->tp_free((PyObject *) self);
    }

    static PyObject *
    DateTime_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;

        DateTimeObject *self;
        self = (DateTimeObject *) type->tp_alloc(type, 0);
        if (self != NULL) {
            self->dt = nullptr;
        }
        return (PyObject *) self;
    }

    static int
    DateTime_init(DateTimeObject *self, PyObject *args, PyObject *kwds)
    {
        try {
            static char const *kwlist[] = {"datetime", "tzmap", "tzid", NULL};
            const char *dt_str = NULL;
            PyObject *tzmap_obj = NULL;
            const char *tzid_cstr = NULL;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|Os:__init__", (char**)kwlist,
                                            &dt_str, &tzmap_obj, &tzid_cstr))
                return -1;

            if (dt_str == NULL) {
                PyErr_SetString(PyExc_TypeError, "datetime must be a string");
                return -1;
            }

            // optional tzmap support
            if (tzid_cstr) {
                if (!tzmap_obj || !PyObject_TypeCheck(tzmap_obj, &TZMapType)) {
                    PyErr_SetString(PyExc_TypeError, "tzid requires tzmap=uical.TZMap");
                    return -1;
                }
                TZMapObject* tzm = reinterpret_cast<TZMapObject*>(tzmap_obj);
                std::string combined(dt_str);
                combined += tzid_cstr;
                self->dt = uICAL::new_ptr<uICAL::DateTime>(combined, tzm->tzmap);
            }
            else if (tzmap_obj && PyObject_TypeCheck(tzmap_obj, &TZMapType)) {
                TZMapObject* tzm = reinterpret_cast<TZMapObject*>(tzmap_obj);
                self->dt = uICAL::new_ptr<uICAL::DateTime>(std::string(dt_str), tzm->tzmap);
            }
            else if (tzmap_obj && tzmap_obj != Py_None) {
                PyErr_SetString(PyExc_TypeError, "tzmap must be a uical.TZMap or None");
                return -1;
            }
            else {
                self->dt = uICAL::new_ptr<uICAL::DateTime>(std::string(dt_str));
            }

            return 0;
        }
        catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return -1;
        }
    }

    static PyObject *
    DateTime_str(PyObject *s) {
        DateTimeObject* self = (DateTimeObject*)s;
        try {
            return PyUnicode_FromString(self->dt->as_str().c_str());
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }

    static PyObject *
    DateTime_as_tuple(DateTimeObject *self, PyObject *Py_UNUSED(ignored))
    {
        try {
            uICAL::DateStamp ds = self->dt->datestamp();
            PyObject* ret = PyTuple_New(7);
            PyTuple_SetItem(ret, 0, PyLong_FromLong(ds.year));
            PyTuple_SetItem(ret, 1, PyLong_FromLong(ds.month));
            PyTuple_SetItem(ret, 2, PyLong_FromLong(ds.day));
            PyTuple_SetItem(ret, 3, PyLong_FromLong(ds.hour));
            PyTuple_SetItem(ret, 4, PyLong_FromLong(ds.minute));
            PyTuple_SetItem(ret, 5, PyLong_FromLong(ds.second));
            PyTuple_SetItem(ret, 6, PyUnicode_FromString(self->dt->tz->as_str().c_str()));
            return ret;
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyMethodDef DateTime_methods[] = {
        {"as_tuple", (PyCFunction)DateTime_as_tuple, METH_NOARGS,
            "Return (year, month, day, hour, minute, second, tz)"},
        {NULL, NULL, 0, NULL}
    };

    PyTypeObject DateTimeType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.DateTime",
        .tp_basicsize = sizeof(DateTimeObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) DateTime_dealloc,
        .tp_str = DateTime_str,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "DateTime wrapper for uICAL",
        .tp_methods = DateTime_methods,
        .tp_init = (initproc) DateTime_init,
        .tp_new = DateTime_new,
    };

    uICAL::DateTime_ptr DateTime_from_py(PyObject* obj, const char* where)
    {
        if (PyObject_TypeCheck(obj, &DateTimeType)) {
            DateTimeObject* dtobj = reinterpret_cast<DateTimeObject*>(obj);
            return dtobj->dt;
        }
        PyErr_Format(PyExc_TypeError, "%s must be a uical.DateTime", where);
        return nullptr;
    }

    bool PyInit_DateTime(PyObject* module)
    {
        if (PyType_Ready(&DateTimeType) < 0)
            return false;

        Py_INCREF(&DateTimeType);
        if (PyModule_AddObject(module, "DateTime", (PyObject *) &DateTimeType) < 0) {
            Py_DECREF(&DateTimeType);
            return false;
        }

        return true;
    }
}
