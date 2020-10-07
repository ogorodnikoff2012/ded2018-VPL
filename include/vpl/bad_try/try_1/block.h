#pragma once

#include <vector>
#include <string>
#include <memory>

#include "variable_declaration.h"
#include "statement.h"
#include "semantic_node.h"

namespace vpl {

class Block : public SemanticNode {
public:
private:
    std::vector<std::unique_ptr<VariableDeclaration>> variables_;
    std::vector<std::unique_ptr<Statement>> statements_;
};

}  /* namespace vpl */
