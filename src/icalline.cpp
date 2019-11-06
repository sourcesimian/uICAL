#include "uICAL/cppstl.h"
#include "uICAL/error.h"
#include "uICAL/icalline.h"
#include "uICAL/util.h"

namespace uICAL {
    VLine::ptr VLine::init(const std::string line) {
        return VLine::ptr(new VLine(line));
    }

    VLine::VLine(const std::string line) {
        size_t colon = line.find(":");
        size_t semicolon = line.find(";");

        if (colon == std::string::npos) {
            throw ParseError(std::string("\n!BAD LINE: ") + line);
        }
        
        if (semicolon != std::string::npos && semicolon < colon) {
            this->name = line.substr(0, semicolon);
            this->readParams(line.substr(semicolon + 1, colon - semicolon - 1));
        }
        else {
            this->name = line.substr(0, colon);
        }
        this->value = line.substr(colon + 1, line.length() - colon - 1);
    }

    std::string VLine::getParam(const std::string key) {
        for (auto it = this->params.begin(); it != this->params.end(); ++it) {
            if (it->first == key) {
                return it->second;
            }
        }
        return std::string("");
    }

    void VLine::readParams(const std::string str) {
        tokenize(str, ';', [&](const std::string token){
            size_t equals = token.find("=");
            if (equals == std::string::npos) {
                throw ParseError(std::string("\n!BAD PARAM: ") + token);
            }
            const std::string name = token.substr(0, equals);
            const std::string value = token.substr(equals + 1, token.length());
            this->params.insert(std::pair<std::string, std::string>(name, value));
        });
    }

    std::ostream & operator << (std::ostream &out, const VLine::ptr &l) {
        l->str(out);
        return out;
    }

    std::ostream & operator << (std::ostream &out, const VLine &l) {
        l.str(out);
        return out;
    }

    void VLine::str(std::ostream& out) const {
        out << this->name;
        if (this->params.size()) {
            for (auto kv : this->params) {
                out << ";";
                out << kv.first << "=" << kv.second ;
            }
        }
        out << ":" << this->value;
    }

    VLineReader::VLineReader() {
        this->line = 0;
    }

    const VLine::ptr VLineReader::next() {
        VLine::ptr ret = this->peek();
        this->pop();
        return ret;
    }

    VLineReaderStream::VLineReaderStream(std::istream& ical)
    : ical(ical)
    {
        this->current.clear();
    }

    const VLine::ptr VLineReaderStream::peek() {
        if (this->ical.eof()) {
            throw VLineReaderEnd();
        }
        if (this->current.empty()) {
            std::string token;
            while(std::getline(this->ical, token, '\n')) {
                if (this->current.empty()) {
                    this->current = token;
                }
                else {
                    this->current += token;
                }
                this->line ++;
                if (this->ical.peek() != ' ') {
                    break;
                }
                else {
                    this->ical.get();
                }
            }
        }
        return VLine::init(this->current);
    }

    void VLineReaderStream::pop() {
        this->current.clear();
    }
}