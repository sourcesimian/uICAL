/*############################################################################
# Copyright (c) 2025 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/vline.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "vline.h"

namespace uical_python {

    static void
    VLine_dealloc(VLineObject *self)
    {
        self->vline = nullptr;
        Py_TYPE(self)->tp_free((PyObject *) self);
    }

    static PyObject *
    VLine_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;

        VLineObject *self;
        self = (VLineObject *) type->tp_alloc(type, 0);
        if (self != NULL) {
            self->vline = nullptr;
        }
        return (PyObject *) self;
    }

    static int
    VLine_init(VLineObject *self, PyObject *args, PyObject *kwds)
    {
        try {
            static char const *kwlist[] = {"line", NULL};
            const char *line = NULL;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:__init__", (char**)kwlist, &line))
                return -1;

            if (line == NULL) {
                PyErr_SetString(PyExc_TypeError, "line must be a string");
                return -1;
            }

            self->vline = uICAL::new_ptr<uICAL::VLine>(std::string(line));
            return 0;
        }
        catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return -1;
        }
    }

    static PyObject *
    VLine_str(PyObject *s) {
        VLineObject* self = (VLineObject*)s;
        try {
            return PyUnicode_FromString(self->vline->as_str().c_str());
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }

    static PyObject *
    VLine_get_name(VLineObject *self, void *closure)
    {
        std::ignore = closure;
        return PyUnicode_FromString(self->vline->name.c_str());
    }

    static PyObject *
    VLine_get_value(VLineObject *self, void *closure)
    {
        std::ignore = closure;
        return PyUnicode_FromString(self->vline->value.c_str());
    }

    static PyObject *
    VLine_get_param(VLineObject *self, PyObject *args)
    {
        const char* key = nullptr;
        if (!PyArg_ParseTuple(args, "s", &key)) {
            return nullptr;
        }
        try {
            std::string val = self->vline->getParam(std::string(key));
            return PyUnicode_FromString(val.c_str());
        } catch (uICAL::Error& ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return nullptr;
        }
    }

    static PyGetSetDef VLine_getset[] = {
        { (char*)"name", (getter)VLine_get_name, NULL, (char*)"property name", NULL },
        { (char*)"value", (getter)VLine_get_value, NULL, (char*)"property value", NULL },
        { NULL }  /* Sentinel */
    };

    static PyMethodDef VLine_methods[] = {
        {"getParam", (PyCFunction) VLine_get_param, METH_VARARGS,
            "Get parameter value by key"},
        {NULL, NULL, 0, NULL}
    };

    PyTypeObject VLineType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.VLine",
        .tp_basicsize = sizeof(VLineObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) VLine_dealloc,
        .tp_str = VLine_str,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Wrapper for uICAL::VLine",
        .tp_methods = VLine_methods,
        .tp_getset = VLine_getset,
        .tp_init = (initproc) VLine_init,
        .tp_new = VLine_new,
    };

    bool PyInit_VLine(PyObject* module)
    {
        if (PyType_Ready(&VLineType) < 0)
            return false;

        Py_INCREF(&VLineType);
        if (PyModule_AddObject(module, "VLine", (PyObject *) &VLineType) < 0) {
            Py_DECREF(&VLineType);
            return false;
        }

        return true;
    }
}
