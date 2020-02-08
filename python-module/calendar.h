#ifndef python_calendar_h
#define python_calendar_h

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::CalendarIter::ptr calendar;
    } CalendarObject;

    bool PyInit_Calendar(PyObject* module);
}
#endif
