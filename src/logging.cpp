/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/logging.h"

#if defined(UICAL_LOG_LEVEL)
    #if UICAL_LOG_LEVEL > 0
        #if defined(ARDUINO)
            #include <Arduino.h>
        #else
            #include <iostream>
        #endif
    #endif
#endif


namespace uICAL {
    #if defined(UICAL_LOG_LEVEL)
        #if UICAL_LOG_LEVEL > 0
            void _logging_null(const char *fmt, ...) {
                std::ignore = fmt;
            }

            #if defined(ARDUINO)
                const char* _logging_level_str(_logging_level level) {
                    switch(level) {
                        case _logging_level::trace:      return "T";
                        case _logging_level::debug:      return "D";
                        case _logging_level::info:       return "I";
                        case _logging_level::warn:       return "W";
                        case _logging_level::error:      return "E";
                        default: return "?";
                    }
                }

                #if !defined(UICAL_LOG_HANDLER)
                    void _logging_handler(const char* log){
                        Serial.println(log);
                    }
                #endif
            #else
                const char* _logging_level_str(_logging_level level) {
                    switch(level) {
                        case _logging_level::trace:      return "TRACE";
                        case _logging_level::debug:      return "DEBUG";
                        case _logging_level::info:       return "INFO";
                        case _logging_level::warn:       return "WARNING";
                        case _logging_level::error:      return "ERROR";
                        default: return "???";
                    }
                }

                #if !defined(UICAL_LOG_HANDLER)
                    void _logging_handler(const char* log){
                        std::cout << log << std::endl;
                    }
                #endif
            #endif
        #endif
    #endif
}
