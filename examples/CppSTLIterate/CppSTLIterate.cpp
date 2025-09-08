// This example is derived from the README.md example 'Get All Events Between Two Times (C++ STL)'.
// When using the ica1l.txt in this repository it returns the 'start' and 'summary' properties for 10 resulting recuring events iterated from within the coded date range.
// If error at runtime ensure file ical1.txt can be found and is valid.

// #include <fstream> // could uncomment if prefering this Standard library

#include <uICAL.h>

int main() {
    std::ifstream fstm(std::string("test/data/ical1.txt"));

    uICAL::istream_stl istm(fstm);
    auto cal = uICAL::Calendar::load(istm);

    uICAL::DateTime begin("20190901T102000Z");
    uICAL::DateTime end("20190931T103000Z");

    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, begin, end);

    while (calIt->next()) {
        std::cout << calIt->current()->start().as_str() << " " << calIt->current()->summary() << std::endl;
    }
}
