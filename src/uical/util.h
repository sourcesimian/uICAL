#ifndef uical_util_h
#define uical_util_h

namespace uICAL {
    void tokenize(const std::string input, char token, std::function<void (std::string)> cb);
    int string_to_int(const std::string input);

    class Joiner {
        public:
            Joiner(char delim);
            std::ostringstream& out();
            Joiner& next();

            std::string str() const;
            void str(std::ostream& out) const;
            void write(std::ostream& out) const;
        private:
            char delim;
            std::ostringstream current;
            std::vector<std::string> values;
    };

    template <typename A, typename B>
    void unpack(std::pair<A, B> p, A &a, B &b) {
        a = std::get<0>(p);
        b = std::get<1>(p);
    };

    template <typename I>
    std::vector<I> toVector(const std::string& value) {
        std::vector<I> v;
        tokenize(value, ',', [&](const std::string part){
            v.push_back((I)string_to_int(part));
        });
        return v;
    }

    template <typename I>
    std::ostream & operator << (std::ostream &out, const std::vector<I> &ia) {
        Joiner values(',');
        
        for (I value : ia) {
            values.out() << value;
            values.next();
        }
        values.str(out);
        return out;
    }
}
#endif
