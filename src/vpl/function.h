#pragma once

#include <vpl/function.h>
#include <vpl/bad_node_error.h>
#include <macros.h>

namespace vpl {

void Function::SetHeader(std::unique_ptr<SemanticNode>&& header) {
    TRY_MOVE(FunctionDeclaration, header, header_);
    BAD_NODE_ERROR();
}

void Function::SetBody(std::unique_ptr<SemanticNode>&& body) {
    TRY_MOVE(Block, body, body_);
    BAD_NODE_ERROR();
}

}
