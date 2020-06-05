#include "../catch.hpp"

#include <cstdio>
#include <iostream>
#include <map>

#include "uICAL.h"

#include "uICAL/datetime.h"

TEST_CASE("DateTime::str", "[uICAL][DateTime]") {
    REQUIRE_THROWS_WITH(uICAL::DateTime("20200605T06493"), "ValueError: Bad datetime: \"20200605T06493\"");
}
