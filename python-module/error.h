#ifndef python_error_h
#define python_error_h

namespace uical_python {
    #ifndef python_error_c
        extern PyObject *UicalError;
    #endif

    bool PyInit_Error(PyObject* module);
    class PyError {
        public:
            PyError(const std::string message, int pyExc)
            : message(message)
            , pyExc(pyExc)
            {
                PyErr_SetString(this->pyExec, this->message.c_str());
            }

        protected:
            const std::string message;
            int pyExc;
    };

}
#endif
