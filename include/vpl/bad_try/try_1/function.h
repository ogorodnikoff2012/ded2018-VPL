#pragma once

#include <memory>
#include <vector>

#include "function_declaration.h"
#include "block.h"
#include "semantic_node.h"

namespace vpl {

class Function : public SemanticNode {
public:
    void SetHeader(std::unique_ptr<SemanticNode>&& header);
    void SetBody(std::unique_ptr<SemanticNode>&& body);

private:
    std::unique_ptr<FunctionDeclaration> header_;
    std::unique_ptr<Block> body_;
};

}  /* namespace vpl */
