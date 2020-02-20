/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_stream_h
#define uical_stream_h

namespace uICAL {
    class stream {
        public:
            stream& operator <<(const stream& stm);
            stream& operator <<(const char* st);
            stream& operator <<(const string& st);

            string str() const;
        protected:
            std::vector<string> strings;
    };
}    

#endif
