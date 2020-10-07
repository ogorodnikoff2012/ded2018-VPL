#pragma once

#include <string>
#include <memory>
#include <vector>

#include "type.h"
#include "variable_declaration.h"
#include "semantic_node.h"

namespace vpl {

class FunctionDeclaration : public SemanticNode {
public:
private:
    std::unique_ptr<Type> return_type_;
    std::string name_;
    std::vector<std::unique_ptr<VariableDeclaration>> args_;
};

}  /* namespace vpl */
