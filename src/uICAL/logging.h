/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_logging_h
#define uical_logging_h

#if defined(UICAL_LOG_LEVEL)
    #if (UICAL_LOG_LEVEL > 0)
        #if defined(ARDUINO)
            #include <Arduino.h>
        #endif
    #endif
#endif
    

namespace uICAL {
    #define UICAL_LOG_LEVEL_ERROR 1
    #define UICAL_LOG_LEVEL_WARN  2
    #define UICAL_LOG_LEVEL_INFO  3
    #define UICAL_LOG_LEVEL_DEBUG 4
    #define UICAL_LOG_LEVEL_TRACE 5

    #if (!defined(UICAL_LOG_LEVEL) || UICAL_LOG_LEVEL == 0)
        #define log_error(...)  (void)0
        #define log_warn(...)   (void)0
        #define log_info(...)   (void)0
        #define log_debug(...)  (void)0
        #define log_trace(...)  (void)0
    #else
        enum class _logging_level {
            error = UICAL_LOG_LEVEL_ERROR,
            warn  = UICAL_LOG_LEVEL_WARN,
            info  = UICAL_LOG_LEVEL_INFO, 
            debug = UICAL_LOG_LEVEL_DEBUG,
            trace = UICAL_LOG_LEVEL_TRACE,
        };

        void _logging_null(const char *fmt, ...);
        const char* _logging_level_str(_logging_level level);

        #if !defined(UICAL_LOG_HANDLER)
            void _logging_handler(const char* log);
        #endif

        template<typename... Args>
        void _logging_msg(_logging_level level,
                            const char* file, int lineNo, const char* func,
                            const char *fmt, Args... args) {
            const int max = 255;
            char buf[max];
            int n = snprintf(buf, max-1, "[uICAL:%c] [%s:%d] %s(): ", _logging_level_str(level)[0], file, lineNo, func);
            snprintf(buf + n, max-1-n, fmt, args...);
            _logging_handler(buf);
        }

        #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_ERROR)
            #define log_error(...)      _logging_msg(_logging_level::error, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_error(...)      (void)0
        #endif 

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_WARN)
            #define log_warning(...)    _logging_msg(_logging_level::warn, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_warning(...)    (void)0
        #endif 

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_INFO)
            #define log_info(...)       _logging_msg(_logging_level::info, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_info(...)       (void)0
        #endif 

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_DEBUG)
            #define log_debug(...)      _logging_msg(_logging_level::debug, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_debug(...)      (void)0
        #endif 

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_TRACE)
            #define log_trace(...)      _logging_msg(_logging_level::trace, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_trace(...)      (void)0
        #endif 

    #endif
}
#endif