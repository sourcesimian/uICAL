#ifndef uical_icalline_h
#define uical_icalline_h

namespace uICAL {
    class VLine {
        public:
            using ptr = std::shared_ptr<VLine>;

            static VLine::ptr init(const std::string line);
            VLine(const std::string line);

            std::string name;
            std::map<std::string, std::string> params;
            std::string value;
            std::string getParam(const std::string key);

            void str(std::ostream &) const;

        private:
            void readParams(const std::string str);
    };

    std::ostream & operator << (std::ostream &out, const VLine::ptr &l);
    std::ostream & operator << (std::ostream &out, const VLine &l);

    class VLineReader {
        public:
            using ptr = std::shared_ptr<VLineReader>;

            VLineReader(); // TODO
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
            VLineReaderStream(std::istream& ical); // TODO
            virtual ~VLineReaderStream() {};
            const VLine::ptr peek();
            void pop();
        protected:
            std::istream& ical;
            std::string current;
    };

    class VLineReaderEnd {
    };
}
#endif
