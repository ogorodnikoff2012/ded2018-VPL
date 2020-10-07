#include <vpl/function_declaration.h>
#include <vpl/program.h>

#include <sstream>

namespace vpl {

void FunctionDeclaration::SetName(const std::string& name) {
    if (arguments_.empty()) {
        name_ = name;
    } else {
        arguments_.back().name = name;
    }
    arguments_.emplace_back();
}

void FunctionDeclaration::Assemble() {
    arguments_.pop_back();
    auto& type_stack = program_->GetTypeStack();
    for (auto iter = arguments_.rbegin(); iter != arguments_.rend(); ++iter) {
        iter->type = type_stack.back();
        type_stack.pop_back();
    }
    return_type_ = type_stack.back();
    type_stack.pop_back();

    // Check names
    for (size_t i = 0; i < arguments_.size(); ++i) {
        for (size_t j = i + 1; j < arguments_.size(); ++j) {
            if (arguments_[i].name == arguments_[j].name) {
                std::stringstream ss;
                ss << "Argument name duplicate: " << arguments_[i].name <<
                    "; in function " << name_;
                throw std::runtime_error(ss.str());
            }
        }
    }
}

bool FunctionDeclaration::IsEquivalent(const FunctionDeclaration* other) const {
    if (!return_type_->IsEquivalent(other->return_type_)) {
        return false;
    }

    if (arguments_.size() != other->arguments_.size()) {
        return false;
    }

    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (!arguments_[i].type->IsEquivalent(other->arguments_[i].type)) {
            return false;
        }
    }

    return true;
}

std::ostream& operator<<(std::ostream& out, const FunctionDeclaration& func_decl) {
    out << *func_decl.return_type_ << ' ' << func_decl.GetName() << '(';
    bool first = true;
    for (const auto& arg : func_decl.arguments_) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << *arg.type << ' ' << arg.name;
    }
    return out << ')';
}

}  /* namespace vpl */
