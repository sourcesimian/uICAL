/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_calendar_h
#define python_calendar_h

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::CalendarIter_ptr calendar;
    } CalendarObject;

    bool PyInit_Calendar(PyObject* module);
}
#endif
