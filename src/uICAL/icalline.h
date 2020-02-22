/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_icalline_h
#define uical_icalline_h

#include "uICAL/base.h"

namespace uICAL {
    class VLine : public Base {
        public:
            using ptr = std::shared_ptr<VLine>;

            static VLine::ptr init();
            static VLine::ptr init(const string& line);
            VLine();
            VLine(const string& line);

            string name;
            std::map<string, string> params;
            string value;
            string getParam(const string& key);

            void str(ostream& out) const;

        private:
            void readParams(const string& str);
    };

    ostream& operator << (ostream& out, const VLine::ptr& l);
    ostream& operator << (ostream& out, const VLine& l);

    class VLineReader {
        public:
            using ptr = std::shared_ptr<VLineReader>;

            VLineReader();
            virtual ~VLineReader() {};
            virtual const VLine::ptr peek() = 0;
            virtual void pop() = 0;
            virtual const VLine::ptr next();
            int lineno() { return this->line; };

        protected:
            int line;
    };

    class VLineReaderStream : public VLineReader {
        public:
            VLineReaderStream(istream& ical);
            virtual ~VLineReaderStream() {};
            const VLine::ptr peek();
            void pop();
        protected:
            istream& ical;
            string current;
    };

    class VLineReaderEnd {
    };
}
#endif
