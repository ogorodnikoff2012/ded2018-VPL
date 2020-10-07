#pragma once

#include "object.h"
#include "type.h"
#include "variable.h"
#include "expression.h"

#include <vector>
#include <unordered_map>
#include <variant>

namespace vpl {

class Block : public Object {
public:
    using Ptr = std::shared_ptr<Block>;

    UNCALLABLE(SetName, const std::string&)
    virtual void AddBlock(Object::Ptr& block) override;
    virtual void AddVariable(Object::Ptr& var) override;
    Variable::Ptr FindVariable(const std::string& str) const {
        Variable::Ptr result;
        auto iter = var_name_map_.find(str);
        if (iter != var_name_map_.end()) {
            result = variables_[iter->second];
        }
        return result;
    }

private:
    std::vector<Variable::Ptr> variables_;
    std::unordered_map<std::string, int> var_name_map_;
    std::vector<std::variant<Block::Ptr, Expression::Ptr>> children_;
};

}  /* namespace vpl */
