/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_util_h
#define uical_util_h

namespace uICAL {
    void tokenize(const string& input, char token, std::function<void (string)> cb);

    class Joiner {
        public:
            Joiner(char delim);
            std::ostringstream& out();
            Joiner& next();

            string str() const;
            void str(std::ostream& out) const;
            void write(std::ostream& out) const;
        private:
            char delim;
            std::ostringstream current;
            std::vector<string> values;
    };

    template <typename A, typename B>
    void unpack(std::pair<A, B> p, A &a, B &b) {
        a = std::get<0>(p);
        b = std::get<1>(p);
    };

    template <typename I>
    std::vector<I> toVector(const string& value) {
        std::vector<I> v;
        tokenize(value, ',', [&](const string part){
            v.push_back((I)part.to_int());
        });
        return v;
    }

    template <typename I>
    std::ostream& operator << (std::ostream& out, const std::vector<I> &ia) {
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
