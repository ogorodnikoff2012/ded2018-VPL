#pragma once

#include <vector>
#include <memory>

#include "semantic_node.h"

namespace vpl {

class Block;

class Statement : public SemanticNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    enum ActionType {
        kActionLoad,
        kActionStore,

    };
private:
    std::vector<std::string> action_args_;
    std::vector<ActionType> actions_;
};

class IfStatement : public Statement {
public:
private:
    std::unique_ptr<ExpressionStatement> condition_;
    std::unique_ptr<Block> true_branch_;
    std::unique_ptr<Block> false_branch_;
};

class WhileStatement : public Statement {
public:
private:
    std::unique_ptr<ExpressionStatement> condition_;
    std::unique_ptr<Block> body_;
};

class DoWhileStatement : public Statement {
public:
private:
    std::unique_ptr<ExpressionStatement> condition_;
    std::unique_ptr<Block> body_;
};

class ForStatement : public Statement {
public:
private:
    std::unique_ptr<ExpressionStatement> init_;
    std::unique_ptr<ExpressionStatement> condition_;
    std::unique_ptr<ExpressionStatement> post_;
    std::unique_ptr<Block> body_;
};

}  /* namespace vpl */
