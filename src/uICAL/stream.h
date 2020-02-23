/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_stream_h
#define uical_stream_h


#ifdef ARDUINO

    #include <Arduino.h>

#else

    #include <istream>

#endif

namespace uICAL {
    class string;
    
    class ostream {
        public:
            ostream& operator <<(const ostream& stm);
            ostream& operator <<(const char* st);
            ostream& operator <<(const string& st);
            ostream& operator <<(unsigned int i);

            operator string() const;

            bool empty() const;
            void clear();

            string str() const;
        protected:
            std::vector<string> strings;
    };

    class istream {
        public:
            virtual ~istream() = default;

            virtual char peek() const = 0;
            virtual char get() = 0;
            virtual bool eof() const = 0;

            virtual bool readuntil(string& st, char delim) = 0;
        protected:
    };

    #ifdef ARDUINO

        class istream_String : public istream {
            public:
                istream_String(const String& istm);

                char peek() const;
                char get();
                bool eof() const;

                bool readuntil(string& st, char delim);

            protected:
                const String& st;
                size_t pos;
        };

    #else

        class istream_stl : public istream {
            public:
                istream_stl(std::istream& istm);

                char peek() const;
                char get();
                bool eof() const;

                bool readuntil(string& st, char delim);

            protected:
                std::istream& istm;
        };

    #endif
}    

#endif
