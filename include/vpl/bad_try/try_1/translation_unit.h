#pragma once

#include <vector>
#include <memory>

#include "semantic_node.h"
#include "type_declaration.h"
#include "variable_declaration.h"
#include "function_declaration.h"
#include "function.h"

namespace vpl {

class TranslationUnit : public SemanticNode {
public:
    void Attach(std::unique_ptr<SemanticNode>&& node);

private:
    std::vector<std::unique_ptr<TypeDeclaration>> type_declarations_;
    std::vector<std::unique_ptr<VariableDeclaration>> variable_declarations_;
    std::vector<std::unique_ptr<FunctionDeclaration>> function_declarations_;
    std::vector<std::unique_ptr<Function>> functions_;
};

}  /* namespace vpl */
