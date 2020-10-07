#pragma once

#include "object.h"
#include "function_declaration.h"
#include "block.h"

namespace vpl {

class Function : public Object {
public:
    void SetHeader(const FunctionDeclaration::Ptr& header) {
        header_ = header;
    }

    UNCALLABLE(SetName, const std::string&)
    UNCALLABLE(AddVariable, Object::Ptr&)

    void SetBody(const Block::Ptr& body) {
        body_ = body;
    }

    virtual void AddBlock(Object::Ptr& obj) override;

private:
    FunctionDeclaration::Ptr header_;
    Block::Ptr body_;
};

}  /* namespace vpl */
