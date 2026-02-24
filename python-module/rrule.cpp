/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/datestamp.h"
#include "uICAL/rrule.h"
#include "uICAL/rruleiter.h"
#include "uICAL/tz.h"

#include <sstream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "rrule.h"
#include "datetime.h"

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
            char *rrule = NULL;
            PyObject *dtstart = NULL, *begin = NULL, *end = NULL, *exclude = NULL;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO|OOO:__init__", (char**)kwlist,
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

            uICAL::DateTime_ptr dtstart_dt = DateTime_from_py(dtstart, "dtstart");
            if (!dtstart_dt) return -1;

            uICAL::DateTime rrBegin; // default constructed
            uICAL::DateTime rrEnd;

            if (begin && begin != Py_None) {
                uICAL::DateTime_ptr begin_dt = DateTime_from_py(begin, "begin");
                if (!begin_dt) return -1;
                rrBegin = *begin_dt;
            }

            if (end && end != Py_None) {
                uICAL::DateTime_ptr end_dt = DateTime_from_py(end, "end");
                if (!end_dt) return -1;
                rrEnd = *end_dt;
            }

            uICAL::RRule_ptr rr = uICAL::new_ptr<uICAL::RRule>(std::string(rrule), *dtstart_dt);
            self->rrule = uICAL::new_ptr<uICAL::RRuleIter>(rr, rrBegin, rrEnd);

            if (exclude && exclude != Py_None) {
                if (PyList_Check(exclude) || PyTuple_Check(exclude)) {
                    PyObject* seq = exclude;
                    Py_ssize_t size = PySequence_Size(seq);
                    for (Py_ssize_t i = 0; i < size; ++i) {
                        PyObject *item = PySequence_GetItem(seq, i);
                        if (!item) return -1;
                        uICAL::DateTime_ptr ex_dt = DateTime_from_py(item, "exclude");
                        Py_DECREF(item);
                        if (!ex_dt) return -1;
                        rr->exclude(*ex_dt);
                    }
                }
                else {
                    uICAL::DateTime_ptr ex_dt = DateTime_from_py(exclude, "exclude");
                    if (!ex_dt) return -1;
                    rr->exclude(*ex_dt);
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
            uICAL::DateTime dt = self->rrule->now();
            DateTimeObject* dto = reinterpret_cast<DateTimeObject*>(
                DateTimeType.tp_alloc(&DateTimeType, 0));
            if (!dto) return NULL;
            dto->dt = uICAL::new_ptr<uICAL::DateTime>(dt);
            return reinterpret_cast<PyObject*>(dto);
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }


    static PyObject *
    RRule__str__(PyObject *s) {
        RRuleObject* self = (RRuleObject*)s;
        try {
            PyObject* ret = PyUnicode_FromString(self->rrule->as_str().c_str());
            return ret;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }            
    }

    static PyMethodDef RRule_methods[] = {
        {"next", (PyCFunction) RRule_next, METH_NOARGS,
         "Test if there is a next value"
        },
        {"now", (PyCFunction) RRule_now, METH_NOARGS,
         "Get the latest value"
        },
        {NULL, NULL, 0, NULL}
    };


    PyTypeObject RRuleType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.RRule",
        .tp_basicsize = sizeof(RRuleObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) RRule_dealloc,
        .tp_str = RRule__str__,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Iterator object for uICAL::RRule",
        .tp_methods = RRule_methods,
        .tp_init = (initproc) RRule_init,
        .tp_new = RRule_new,
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
