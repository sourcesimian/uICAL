/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_rrule_h
#define python_rrule_h

namespace uical_python {
    typedef struct {
        PyObject_HEAD
        uICAL::RRuleIter::ptr rrule;
    } RRuleObject;

    bool PyInit_RRule(PyObject* module);
}
#endif
