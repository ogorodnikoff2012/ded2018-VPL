#pragma once

#include "object.h"
#include "type.h"

#include <vector>
#include <ostream>

namespace vpl {

class Program;

class FunctionDeclaration : public Object {
public:
    using Ptr = std::shared_ptr<FunctionDeclaration>;

    explicit FunctionDeclaration(Program* program) : program_(program) {}
    virtual void SetName(const std::string& name) override;
    UNCALLABLE(AddBlock, Object::Ptr&)
    UNCALLABLE(AddVariable, Object::Ptr&)

    void Assemble();
    const std::string& GetName() const {
        return name_;
    }

    bool IsEquivalent(const FunctionDeclaration* other) const;

private:
    Program* program_;
    Type::Ptr return_type_;
    std::vector<NamedType> arguments_;
    std::string name_;
    friend std::ostream& operator<<(std::ostream&, const FunctionDeclaration&);
};

std::ostream& operator<<(std::ostream& out, const FunctionDeclaration& func_decl);

}  /* namespace vpl */
