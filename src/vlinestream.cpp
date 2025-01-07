/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"

namespace uICAL {
    VLineStream::VLineStream(istream& ical)
    : ical(ical)
    , currentLine(nullptr)
    , repeat(false)
    , bufferedLine()
    {}

    void VLineStream::repeatLine() {
        this->repeat = true;
    }

    bool VLineStream::isFoldedLine(const string& line) {
        return !line.empty() && (line[0] == ' ' || line[0] == '\t');
    }

    const VLine_ptr VLineStream::next() {
        if (this->repeat && this->currentLine != nullptr) {
            this->repeat = false;
            return this->currentLine;
        }

        string line;
        string completeLine;
        bool hasLine = false;

        if (!this->bufferedLine.empty()) {
            completeLine = this->bufferedLine;
            this->bufferedLine.clear();
            hasLine = true;
        }
        else if (line.readfrom(this->ical, '\n')) {
            line.rtrim();
            completeLine = line;
            hasLine = true;
        }

        if (hasLine) {
            string nextLine;
            while (nextLine.readfrom(this->ical, '\n')) {
                nextLine.rtrim();
                if (!isFoldedLine(nextLine)) {
                    this->bufferedLine = nextLine;
                    break;
                }
                completeLine += nextLine.substr(1);
            }

            this->currentLine = new_ptr<VLine>(completeLine);
        } else {
            this->currentLine = nullptr;
        }

        return this->currentLine;
    }
}
