#pragma once

#include "object.h"

namespace vpl {

class Expression : public Object {
public:
    using Ptr = std::shared_ptr<Expression>;

    UNCALLABLE(SetName, const std::string&);
    UNCALLABLE(AddBlock, Object::Ptr&);
    UNCALLABLE(AddVariable, Object::Ptr&);
private:

};

}  /* namespace vpl */
