/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/tzmap.h"
#include "uICAL/stream.h"
#include "uICAL/calendar.h"
#include "uICAL/vobject.h"

#include <fstream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "tzmap.h"

namespace uical_python {
    static void
    TZMap_dealloc(TZMapObject *self)
    {
        self->tzmap = nullptr;
        Py_TYPE(self)->tp_free((PyObject *) self);
    }

    static PyObject *
    TZMap_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;

        TZMapObject *self;
        self = (TZMapObject *) type->tp_alloc(type, 0);
        if (self != NULL) {
            self->tzmap = nullptr;
        }
        return (PyObject *) self;
    }

    static int
    TZMap_init(TZMapObject *self, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;

        try {
            self->tzmap = uICAL::new_ptr<uICAL::TZMap>();

            return 0;
        }
        catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return -1;
        }
    }

    static PyObject *
    TZMap_add(TZMapObject *self, PyObject *args)
    {
        try {
            const char* tzid;
            const char* name;
            const char* offset;
            if (!PyArg_ParseTuple(args, "sss", &tzid, &name, &offset)) {
                return NULL;
            }
            self->tzmap->add(std::string(tzid), std::string(name), std::string(offset));
            Py_RETURN_NONE;
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyObject *
    TZMap_from_file(TZMapObject *self, PyObject *args)
    {
        const char* path = nullptr;
        if (!PyArg_ParseTuple(args, "s", &path)) {
            return NULL;
        }

        // std::ifstream input(path);
        // uICAL::istream_stl input(path);
        // if (!input.is_open()) {
        //     PyErr_Format(PyExc_IOError, "Cannot open %s", path);
        //     return NULL;
        // }

        try {
            std::ifstream input(path);
            if (!input.is_open()) {
                PyErr_Format(PyExc_IOError, "Cannot open %s", path);
                return NULL;
            }

            uICAL::istream_stl ical(input);

            auto skipEvents = [](const uICAL::VEvent&) { return false; };
            uICAL::Calendar::load(ical, self->tzmap, skipEvents);

            Py_RETURN_NONE;
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyObject *
    TZMap_get_offset(TZMapObject *self, PyObject *args)
    {
        try {
            const char* tzid;
            if (!PyArg_ParseTuple(args, "s", &tzid)) {
                return NULL;
            }
            int offset = self->tzmap->getOffset(std::string(tzid));
            return PyLong_FromLong(offset);
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyObject *
    TZMap_get_name(TZMapObject *self, PyObject *args)
    {
        try {
            const char* tzid;
            if (!PyArg_ParseTuple(args, "s", &tzid)) {
                return NULL;
            }
            std::string name = self->tzmap->getName(std::string(tzid));
            return PyUnicode_FromString(name.c_str());
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyObject *
    TZMap_find_id(TZMapObject *self, PyObject *args)
    {
        try {
            const char* nameOrId;
            if (!PyArg_ParseTuple(args, "s", &nameOrId)) {
                return NULL;
            }
            std::string id = self->tzmap->findId(std::string(nameOrId));
            return PyUnicode_FromString(id.c_str());
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return NULL;
        }
    }

    static PyObject *
    TZMap__str__(PyObject *s) {
        TZMapObject* self = (TZMapObject*)s;
        try {
            PyObject* ret = PyUnicode_FromString(self->tzmap->as_str().c_str());
            return ret;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }

    static PyMethodDef TZMap_methods[] = {
        {"add", (PyCFunction)TZMap_add, METH_VARARGS,
            "Add a time zone by ID, name, and offset"
        },
        {"getOffset", (PyCFunction)TZMap_get_offset, METH_VARARGS,
            "Get offset by TZID"
        },
        {"getName", (PyCFunction)TZMap_get_name, METH_VARARGS,
            "Get name by TZID"
        },
        {"findId", (PyCFunction)TZMap_find_id, METH_VARARGS,
            "Find TZID by name or ID"
        },
        {"fromFile", (PyCFunction)TZMap_from_file, METH_VARARGS,
            "Load VTIMEZONE components from an .ics file into this TZMap"},
        {NULL, NULL, 0, NULL}
    };

    PyTypeObject TZMapType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.TZMap",
        .tp_basicsize = sizeof(TZMapObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) TZMap_dealloc,
        .tp_str = TZMap__str__,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Map of time zones by ID",
        .tp_methods = TZMap_methods,
        .tp_init = (initproc) TZMap_init,
        .tp_new = TZMap_new,
    };

    bool PyInit_TZMap(PyObject* module)
    {
        if (PyType_Ready(&TZMapType) < 0)
            return false;

        Py_INCREF(&TZMapType);
        if (PyModule_AddObject(module, "TZMap", (PyObject *) &TZMapType) < 0) {
            Py_DECREF(&TZMapType);
            return false;
        }

        return true;
    }
}
