#pragma once

#include <string>
#include <memory>

#include "type.h"
#include "semantic_node.h"

namespace vpl {

class VariableDeclaration : public SemanticNode {
public:
private:
    std::unique_ptr<Type> type_;
    std::string name_;
};

}  /* namespace vpl */
