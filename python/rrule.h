#ifndef python_rruleiter_h
#define python_rruleiter_h

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::RRule::ptr rrule;
    } RRuleObject;

    bool PyInit_RRule(PyObject* module);
}
#endif
