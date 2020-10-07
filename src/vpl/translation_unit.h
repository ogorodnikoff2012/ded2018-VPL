#include <vpl/bad_node_error.h>
#include <vpl/translation_unit.h>

namespace vpl {

void TranslationUnit::Attach(std::unique_ptr<SemanticNode>&& node) {
    if (Is<TypeDeclaration>(node)) {
        type_declarations_.emplace_back(std::move(node));
    } else if (Is<VariableDeclaration>(node)) {
        variable_declarations_.emplace_back(std::move(node));
    } else if (Is<FunctionDeclaration>(node)) {
        function_declarations_.emplace_back(std::move(node));
    } else if (Is<Function>(node)) {
        functions_.emplace_back(std::move(node));
    } else {
        BAD_NODE_ERROR();
    }
}

}
