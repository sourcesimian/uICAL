/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef python_error_h
#define python_error_h

namespace uical_python {
    #ifndef python_error_c
        extern PyObject *UicalError;
    #endif

    bool PyInit_Error(PyObject* module);
}
#endif
