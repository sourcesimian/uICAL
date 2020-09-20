/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <sstream>

#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/calendarentry.h"
#include "uICAL/calendariter.h"
#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/stream.h"
#include "uICAL/tz.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "error.h"
#include "calendar.h"

namespace uical_python {
    static void
    Calendar_dealloc(CalendarObject *self)
    {
        self->calendar = nullptr;
        Py_TYPE(self)->tp_free((PyObject *) self);
    }


    static PyObject *
    Calendar_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
    {
        std::ignore = args;
        std::ignore = kwds;

        CalendarObject *self;
        self = (CalendarObject *) type->tp_alloc(type, 0);
        if (self != NULL) {
            self->calendar = nullptr;
        }
        return (PyObject *) self;
    }


    int
    Calendar_init(CalendarObject *self, PyObject *args, PyObject *kwds)
    {
        try {
            static char const *kwlist[] = {"ical", "begin", "end", NULL};
            char *ical = NULL;
            PyObject *begin = NULL, *end = NULL;

            if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|OO:__init__", (char**)kwlist,
                                            &ical,
                                            &begin, &end))
                return -1;

            if (ical == NULL) {
                PyErr_SetString(PyExc_AttributeError, "ical");
                return -1;
            }

            std::istringstream _ical(ical);
            uICAL::istream_stl inp(_ical);
            uICAL::Calendar_ptr cal = uICAL::Calendar::load(inp);

            uICAL::DateTime calBegin;
            uICAL::DateTime calEnd;
            if (begin && PyUnicode_Check(begin)) {
                calBegin = uICAL::DateTime(std::string(PyUnicode_AsUTF8(begin)));
            }
            if (end && PyUnicode_Check(end)) {
                calEnd = uICAL::DateTime(std::string(PyUnicode_AsUTF8(end)));
            }

            self->calendar = uICAL::new_ptr<uICAL::CalendarIter>(cal, calBegin, calEnd);

            return 0;
        }
        catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return -1;
        }
    }


    static PyObject *
    Calendar_next(CalendarObject *self, PyObject *Py_UNUSED(ignored))
    {
        try {
            if (self->calendar->next()) {
                Py_RETURN_TRUE;
            }
            Py_RETURN_FALSE;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }


    static PyObject *
    Calendar_current(CalendarObject *self, PyObject *Py_UNUSED(ignored))
    {
        try {
            uICAL::CalendarEntry_ptr entry = self->calendar->current();

            uICAL::DateTime dtStart = entry->start();
            uICAL::DateStamp dsStart = dtStart.datestamp();

            PyObject* start = PyTuple_New(7);
            PyTuple_SetItem(start, 0, PyLong_FromLong(dsStart.year));
            PyTuple_SetItem(start, 1, PyLong_FromLong(dsStart.month));
            PyTuple_SetItem(start, 2, PyLong_FromLong(dsStart.day));
            PyTuple_SetItem(start, 3, PyLong_FromLong(dsStart.hour));
            PyTuple_SetItem(start, 4, PyLong_FromLong(dsStart.minute));
            PyTuple_SetItem(start, 5, PyLong_FromLong(dsStart.second));
            PyTuple_SetItem(start, 6, PyLong_FromLong(dtStart.tz->offset()));

            PyObject* ret = PyDict_New();
            PyDict_SetItemString(ret, "type", PyUnicode_FromString(uICAL::CalendarEntry::asString(entry->type()).c_str()));
            PyDict_SetItemString(ret, "summary", PyUnicode_FromString(entry->summary().c_str()));
            PyDict_SetItemString(ret, "start", start);
            PyDict_SetItemString(ret, "duration", PyLong_FromLong((entry->end() - entry->start()).totalSeconds()));
            return ret;
        } catch (uICAL::Error ex) {
            PyErr_SetString(UicalError, ex.message.c_str());
            return 0;
        }
    }


    static PyMethodDef Calendar_methods[] = {
        {"next", (PyCFunction) Calendar_next, METH_NOARGS,
         "Test if there is a next value"
        },
        {"current", (PyCFunction) Calendar_current, METH_NOARGS,
         "Get the latest value"
        },
        {NULL, NULL, 0, NULL}
    };



    static PyTypeObject CalendarType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "uical.Calendar",
        .tp_basicsize = sizeof(CalendarObject),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) Calendar_dealloc,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Iterator object for uICAL::Calendar",
        .tp_methods = Calendar_methods,
        .tp_init = (initproc) Calendar_init,
        .tp_new = Calendar_new,
    };


    bool PyInit_Calendar(PyObject* module) {
        if (PyType_Ready(&CalendarType) < 0)
            return false;

        Py_INCREF(&CalendarType);
        if (PyModule_AddObject(module, "Calendar", (PyObject *) &CalendarType) < 0) {
            Py_DECREF(&CalendarType);  // TODO error: use of undeclared identifier 'PY_DECREF'
            return false;
        }

        return true;
    }
}
