#include <sstream>

#include "uICAL/error.h"
#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/rruleiter.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "rrule.h"

namespace uical_python {
    static void
    RRule_dealloc(RRuleObject *self)
    {
        self->rrule = nullptr;
        Py_TYPE(self)->tp_free((PyObject *) self);
    }


    static PyObject *
    RRule_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;
        
        RRuleObject *self;
        self = (RRuleObject *) type->tp_alloc(type, 0);
        if (self != NULL) {
            self->rrule = nullptr;
        }
        return (PyObject *) self;
    }


    int
    RRule_init(RRuleObject *self, PyObject *args, PyObject *kwds)
    {
        try {
            static char const *kwlist[] = {"rrule", "dtstart", "begin", "end", "exclude", NULL};
            char *rrule = NULL, *dtstart = NULL;
            PyObject *begin = NULL, *end = NULL, *exclude = NULL;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss|OOO:__init__", (char**)kwlist,
                                            &rrule, &dtstart,
                                            &begin, &end, &exclude))
                return -1;

            if (rrule == NULL) {
                PyErr_SetString(PyExc_AttributeError, "rrule");
                return -1;
            }

            if (dtstart == NULL) {
                PyErr_SetString(PyExc_AttributeError, "dtstart");
                return -1;
            }

            uICAL::DateTime dt = uICAL::DateTime(std::string(dtstart));
            uICAL::RRule::ptr rr = uICAL::RRule::init(std::string(rrule), dt);
            self->rrule = uICAL::RRuleIter::init(rr);
            
            if (begin && PyUnicode_Check(begin)) {
                self->rrule->begin(uICAL::DateTime(std::string(PyUnicode_AsUTF8(begin))));
            }

            if (end && PyUnicode_Check(end)) {
                self->rrule->end(uICAL::DateTime(std::string(PyUnicode_AsUTF8(end))));
            }

            if (exclude) {
                if (PyList_Check(exclude)) {
                    for (int i = 0; i < PyList_Size(exclude); ++i) {
                        PyObject *item = PyList_GetItem(exclude, i);
                        rr->exclude(uICAL::DateTime(std::string(PyUnicode_AsUTF8(item))));
                    }
                }
                else {
                    rr->exclude(uICAL::DateTime(std::string(PyUnicode_AsUTF8(exclude))));
                }
            }

            return 0;
        }
        catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return -1;
        }
    }


    static PyObject *
    RRule_next(RRuleObject *self, PyObject *Py_UNUSED(ignored))
    {
        try {
            if (self->rrule->next()) {
                Py_RETURN_TRUE;
            }
            Py_RETURN_FALSE;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }


    static PyObject *
    RRule_now(RRuleObject *self, PyObject *Py_UNUSED(ignored))
    {
        try {
            std::ostringstream res;
            // res << self->rrule->now();
            // return PyUnicode_FromString(res.str().c_str());
            PyObject* ret = PyTuple_New(6);

            uICAL::DateStamp now = self->rrule->now().datestamp();
            PyTuple_SetItem(ret, 0, PyLong_FromLong(now.year));
            PyTuple_SetItem(ret, 1, PyLong_FromLong(now.month));
            PyTuple_SetItem(ret, 2, PyLong_FromLong(now.day));
            PyTuple_SetItem(ret, 3, PyLong_FromLong(now.hour));
            PyTuple_SetItem(ret, 4, PyLong_FromLong(now.minute));
            PyTuple_SetItem(ret, 5, PyLong_FromLong(now.second));
            return ret;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }


    static PyMethodDef RRule_methods[] = {
        {"next", (PyCFunction) RRule_next, METH_NOARGS,
        "Test is there is a next value"
        },
        {"now", (PyCFunction) RRule_now, METH_NOARGS,
        "Get the next value"
        },
        {NULL}
    };



    static PyTypeObject RRuleType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.RRule",
        .tp_doc = "Iterator object for uicalRRule",
        .tp_basicsize = sizeof(RRuleObject),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = RRule_new,
        .tp_init = (initproc) RRule_init,
        .tp_dealloc = (destructor) RRule_dealloc,
        .tp_methods = RRule_methods,
    };


    bool PyInit_RRule(PyObject* module) {
        if (PyType_Ready(&RRuleType) < 0)
            return false;

        Py_INCREF(&RRuleType);
        if (PyModule_AddObject(module, "RRule", (PyObject *) &RRuleType) < 0) {
            Py_DECREF(&RRuleType);  // TODO error: use of undeclared identifier 'PY_DECREF'
            return false;
        }

        return true;
    }
}
