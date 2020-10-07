#pragma once

#include <sstream>
#include <stdexcept>
#include <iomanip>

#define REQUIRE(cond) if (!( cond )) {                                                              \
    std::stringstream ss;                                                                           \
    ss << "Requirement failed: " << std::quoted( #cond ) << "; " << __FILE__ << ':' << __LINE__;    \
    throw std::runtime_error(ss.str());                                                             \
}
