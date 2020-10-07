#pragma once

#include <string>
#include <memory>
#include <sstream>

namespace vpl {

class Object {
public:
    using Ptr = std::unique_ptr<Object>;

    virtual void SetName(const std::string& name) = 0;
    virtual void AddBlock(Ptr& block) = 0;
    virtual void AddVariable(Ptr& var) = 0;

    virtual ~Object() = default;
};

#define UNCALLABLE(name, ...)                                       \
virtual void name ( __VA_ARGS__ ) override {                        \
    std::stringstream ss;                                           \
    ss << '`' << __PRETTY_FUNCTION__ << "` shouldn't be called";    \
    throw std::runtime_error(ss.str());                             \
}

}  /* namespace vpl */
