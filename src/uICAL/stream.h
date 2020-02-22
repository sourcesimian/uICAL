/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_stream_h
#define uical_stream_h

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
            istream(const char* st);

            char peek() const { return '?'; }
            char get() { return '?'; }

            bool eof() const { return true; }

            bool readuntil(string& st, char delim);
        protected:
            
    };
}    

#endif
