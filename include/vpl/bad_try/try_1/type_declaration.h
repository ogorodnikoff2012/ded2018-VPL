#pragma once

#include <string>
#include <memory>

#include "type.h"
#include "semantic_node.h"

namespace vpl {

class TypeDeclaration : public SemanticNode {
public:
private:
    std::string name_;
    std::unique_ptr<Type> type_;
};

}  /* namespace vpl */
