/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_error_h
#define uical_error_h

namespace uICAL {
    class Error {
        public:
            std::string message;
    };
    
    class ParseError : public Error {
        public:
            ParseError(std::string m) {
                this->message = std::string("ParseError: ") + m;
            }
    };

    class ValueError : public Error {
        public:
            ValueError(std::string m) {
                this->message = std::string("ValueError: ") + m;
            }
    };

    class NotImplementedError : public Error {
        public:
            NotImplementedError(std::string m) {
                this->message = std::string("NotImplementedError: ") + m;
            }
    };

    class ImplementationError : public Error {
        public:
            ImplementationError(std::string m) {
                this->message = std::string("ImplementationError: ") + m;
            }
    };

    class ICalError : public Error {
        public:
            ICalError(std::string m) {
                this->message = std::string("ICalError: ") + m;
            }
    };

    class RecurrenceError : public Error {
        public:
            RecurrenceError(std::string m) {
                this->message = std::string("RecurrenceError: ") + m;
            }
    };

    class TZAwarenessConflictError : public Error {
        public:
            TZAwarenessConflictError(std::string m) {
                this->message = std::string("TZAwarenessConflictError: ") + m;
            }
    };

}
#endif