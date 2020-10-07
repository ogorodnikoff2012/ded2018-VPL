#pragma once

#include <stdexcept>
#include <sstream>

namespace vpl {

class BadNodeError : public std::runtime_error {
public:
    explicit BadNodeError(const std::string& str) : std::runtime_error(str) {
    }

    static BadNodeError Build(const char* func, int line) {
        std::stringstream ss;
        ss << "Bad node; caught at " << func << ":" << line;
        return BadNodeError(ss.str());
    }
};

#define BAD_NODE_ERROR() vpl::BadNodeError::Build(__PRETTY_FUNCTION__, __LINE__)

}  /* namespace vpl */
