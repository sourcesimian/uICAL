/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_util_h
#define uical_util_h

namespace uICAL {
    class Joiner {
        public:
            Joiner(char delim);

            ostream& out();
            Joiner& next();

            void str(ostream& out) const;
            void write(ostream& out) const;

        private:
            char delim;
            ostream stm;
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
        value.tokenize(',', [&](const string part){
            v.push_back((I)part.as_int());
        });
        return v;
    }

    template <typename I>
    ostream& operator << (ostream& out, const std::vector<I> &ia) {
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
