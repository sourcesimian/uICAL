#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "uICAL.h"

CATCH_TRANSLATE_EXCEPTION(uICAL::Error& ex) {
    return ex.message;
}
