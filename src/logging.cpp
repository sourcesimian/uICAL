/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/logging.h"

namespace uICAL {
    #if defined(UICAL_LOG_LEVEL)
        #if UICAL_LOG_LEVEL > 0

            void _logging_null(const char *fmt, ...) {
            }

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
                    #if defined(ARDUINO)
                        Serial.println(log);
                    #else
                        std::cout << log << std:endl;
                    #endif
                }
            #endif

        #endif
    #endif
}