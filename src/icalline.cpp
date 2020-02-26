/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/icalline.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"

namespace uICAL {
    VLine::ptr VLine::init() {
        return VLine::ptr(new VLine());
    }

    VLine::ptr VLine::init(const string& line) {
        return VLine::ptr(new VLine(line));
    }

    VLine::VLine() {
    }

    VLine::VLine(const string& line) {
        if(line.empty()) {
            log_error("%s", "VLINE is empty");
            throw ParseError("VLINE is empty");
        }

        size_t colon = line.find(":");
        size_t semicolon = line.find(";");

        if (colon == string::npos) {
            log_error("VLINE does not have a ':' \"%s\"", line.c_str());
            throw ParseError(string("VLINE does not have a ':' \"") + line + "\"");
        }
        
        if (semicolon != string::npos && semicolon < colon) {
            this->name = line.substr(0, semicolon);
            this->readParams(line.substr(semicolon + 1, colon - semicolon - 1));
        }
        else {
            this->name = line.substr(0, colon);
        }
        this->value = line.substr(colon + 1, line.length() - colon - 1);
        log_trace("VLINE %s %s", this->name.c_str(), this->value.c_str());
    }

    string VLine::getParam(const string& key) {
        for (auto param : this->params) {
            if (param.first == key) {
                return param.second;
            }
        }
        return string("");
    }

    void VLine::readParams(const string& str) {
        str.tokenize(';', [&](const string token){
            size_t equals = token.find("=");
            if (equals == string::npos) {
                log_error("Bad param: \"%s\"", token.c_str());
                throw ParseError(string("Bad param: ") + token);
            }
            const string name = token.substr(0, equals);
            const string value = token.substr(equals + 1, token.length());
            this->params.insert(std::pair<string, string>(name, value));
        });
    }

    ostream& operator << (ostream& out, const VLine::ptr& l) {
        l->str(out);
        return out;
    }

    ostream& operator << (ostream& out, const VLine& l) {
        l.str(out);
        return out;
    }

    void VLine::str(ostream& out) const {
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

    VLineReaderStream::VLineReaderStream(istream& ical)
    : ical(ical)
    {
        this->current.clear();
    }

    const VLine::ptr VLineReaderStream::peek() {
        if (this->current.empty()) {
            string token;
            while(token.readfrom(this->ical, '\n')) {
                token.rtrim();

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