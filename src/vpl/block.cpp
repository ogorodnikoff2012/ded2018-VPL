#include <vpl/block.h>
#include <macros.h>
#include <vpl/misc.h>

namespace vpl {

void Block::AddBlock(Object::Ptr& block) {
    if (Is<Block>(block.get())) {
        children_.emplace_back(As<Block>(block.release()));
    } else if (Is<Expression>(block.get())) {
        children_.emplace_back(As<Expression>(block.release()));
    } else {
        throw std::runtime_error("Bad block child");
    }
}

void Block::AddVariable(Object::Ptr& var) {
    REQUIRE(Is<Variable>(var.get()));
    variables_.emplace_back(As<Variable>(var.release()));
    if (var_name_map_.find(variables_.back()->GetName()) != var_name_map_.end()) {
        std::stringstream ss;
        ss << "Name `" << variables_.back()->GetName() << "` is already in use";
        throw std::runtime_error(ss.str());
    }
}

}  /* namespace vpl */
