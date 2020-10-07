#pragma once

#include "object.h"
#include "type.h"

namespace vpl {

class Variable : public Object {
public:
    using Ptr = std::shared_ptr<Variable>;

    virtual void SetName(const std::string& name) override {
        name_ = name;
    }

    const std::string& GetName() const {
        return name_;
    }

    UNCALLABLE(AddBlock, Object::Ptr&)
    UNCALLABLE(AddVariable, Object::Ptr&)

    void SetType(const Type::Ptr& type) {
        type_ = type;
    }

    const Type::Ptr& GetType() const {
        return type_;
    }

private:
    std::string name_;
    Type::Ptr type_;
};

}  /* namespace vpl */
