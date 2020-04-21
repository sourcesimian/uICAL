/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_logging_h
#define uical_logging_h

#if defined(UICAL_LOG_LEVEL)
    #if UICAL_LOG_LEVEL > 0
        #if defined(ARDUINO)
            #include <stdio.h>
            #include <cstring>
            #include <tuple>
        #else
            #include <stdio.h>
            #include <cstring>
            #include <tuple>
        #endif
    #endif
#endif

namespace uICAL {
    #define UICAL_LOGGING_MAX_LEN 256
    #define UICAL_LOG_LEVEL_METRIC   -1
    #define UICAL_LOG_LEVEL_ERROR    1
    #define UICAL_LOG_LEVEL_WARNING  2
    #define UICAL_LOG_LEVEL_INFO     3
    #define UICAL_LOG_LEVEL_DEBUG    4
    #define UICAL_LOG_LEVEL_TRACE    5

    #if (!defined(UICAL_LOG_LEVEL) || (UICAL_LOG_LEVEL == 0))
        #define log_error(...)   (void)0
        #define log_warning(...) (void)0
        #define log_info(...)    (void)0
        #define log_debug(...)   (void)0
        #define log_trace(...)   (void)0
    #else
        enum class _logging_level {
            metric  = UICAL_LOG_LEVEL_METRIC,
            error   = UICAL_LOG_LEVEL_ERROR,
            warning = UICAL_LOG_LEVEL_WARNING,
            info    = UICAL_LOG_LEVEL_INFO,
            debug   = UICAL_LOG_LEVEL_DEBUG,
            trace   = UICAL_LOG_LEVEL_TRACE,
        };

        void _logging_null(const char *fmt, ...);
        const char* _logging_level_str(_logging_level level);

        #if !defined(UICAL_LOG_HANDLER)
            void _logging_handler(const char* log);
        #endif

        template<typename... Args>
        void _logging_msg(_logging_level level,
                          const char* file, int lineNo, const char* func,
                          const char* fmt, Args... args) {
            const int max = UICAL_LOGGING_MAX_LEN;
            char buf[max];
            int n = snprintf(buf, max-1, "[uICAL:%s] [%s:%d] %s(): ", _logging_level_str(level), file, lineNo, func);
            n += snprintf(buf + n, max-1-n, fmt, args...);
            if (n > (max-3)) {
                strcpy(buf+(max-3), " ~");
            }
            #if !defined(UICAL_LOG_HANDLER)
                _logging_handler(buf);
            #else
                UICAL_LOG_HANDLER(buf);
            #endif
        }

        #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_ERROR)
            #define log_error(...)      _logging_msg(uICAL::_logging_level::error, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_error(...)      (void)0
        #endif

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_WARNING)
            #define log_warning(...)    _logging_msg(uICAL::_logging_level::warning, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_warning(...)    (void)0
        #endif

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_INFO)
            #define log_info(...)       _logging_msg(uICAL::_logging_level::info, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_info(...)       (void)0
        #endif

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_DEBUG)
            #define log_debug(...)      _logging_msg(uICAL::_logging_level::debug, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_debug(...)      (void)0
        #endif

        #if (UICAL_LOG_LEVEL >= UICAL_LOG_LEVEL_TRACE)
            #define log_trace(...)      _logging_msg(uICAL::_logging_level::trace, __FILENAME__, __LINE__, __func__, __VA_ARGS__)
        #else
            #define log_trace(...)      (void)0
        #endif

        #define log_metric(...)         _logging_msg(uICAL::_logging_level::metric, __FILENAME__, __LINE__, __func__, "%s %d", __VA_ARGS__)

    #endif
}
#endif
