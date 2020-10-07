#include <vpl/program.h>
#include <vpl/misc.h>
#include <vpl/function_declaration.h>
#include <vpl/block.h>
#include <vpl/expression.h>
#include <macros.h>

#include <iostream>

#define NOT_IMPLEMENTED std::cerr << "Not implemented yet! " << __PRETTY_FUNCTION__ << std::endl;
#define UNDER_CONSTRUCTION std::cerr << "Under construction!  " << __PRETTY_FUNCTION__ << std::endl;

namespace vpl {

Program::~Program() {
}


void Program::CreateFunction() {
    UNDER_CONSTRUCTION;
    REQUIRE(objects_.empty());
    objects_.push_back(std::make_unique<Function>());
}

void Program::EndFunction() {
    UNDER_CONSTRUCTION;
    REQUIRE(!objects_.empty() && Is<Function>(objects_.back().get()));
    Function* func = As<Function>(objects_.back().release());
    objects_.pop_back();

    functions_.emplace_back(func);
}


void Program::DeclareFunction() {
    UNDER_CONSTRUCTION;
 //   REQUIRE(objects_.empty() || Is<Function>(objects_.back()));
    REQUIRE(objects_.empty() || Is<Function>(objects_.back().get()));
    objects_.push_back(std::make_unique<FunctionDeclaration>(this));
}

void Program::EndFunctionDeclaration() {
    UNDER_CONSTRUCTION;
    REQUIRE(!objects_.empty() && Is<FunctionDeclaration>(objects_.back().get()));
    auto func_decl = As<FunctionDeclaration>(objects_.back().release());
    objects_.pop_back();
    func_decl->Assemble();
    AddFunctionDeclaration(func_decl);

    if (!objects_.empty()) {
        REQUIRE(Is<Function>(objects_.back().get()));
        As<Function>(objects_.back().get())->SetHeader(
                function_declarations_[func_decl->GetName()]);
    }
}

void Program::AddFunctionDeclaration(FunctionDeclaration* func_decl) {
    auto iter = function_declarations_.find(func_decl->GetName());
    if (iter == function_declarations_.end()) {
        function_declarations_[func_decl->GetName()].reset(func_decl);
    } else if (!iter->second->IsEquivalent(func_decl)) {
        std::stringstream ss;
        ss << "Function `" << iter->first << "` was already declared";
        throw std::runtime_error(ss.str());
    }
}

void Program::BeginBody() {
    NOT_IMPLEMENTED;
}

void Program::BeginType() {
    UNDER_CONSTRUCTION;
    type_stack_.emplace_back();
}


void Program::CreateBlock() {
    UNDER_CONSTRUCTION;
    REQUIRE(!objects_.empty());
    objects_.push_back(std::make_unique<Block>());
}

void Program::EndBlock() {
    UNDER_CONSTRUCTION;
    REQUIRE(objects_.size() >= 2 && Is<Block>(objects_.back().get()));
    Object::Ptr block = std::move(objects_.back());
    objects_.pop_back();
    REQUIRE(!objects_.empty());
    objects_.back()->AddBlock(block);
}


void Program::CreateExpression() {
    UNDER_CONSTRUCTION;
    objects_.push_back(std::make_unique<Expression>());
}

void Program::EndExpression() {
    UNDER_CONSTRUCTION;
    REQUIRE(!objects_.empty() && Is<Expression>(objects_.back().get()));
    Object::Ptr obj = std::move(objects_.back());
    objects_.pop_back();
    REQUIRE(!objects_.empty());
    objects_.back()->AddBlock(obj);
}


void Program::CreateIfBlock() {
    NOT_IMPLEMENTED;
}

void Program::BeginTrueBranch() {
    NOT_IMPLEMENTED;
}

void Program::BeginFalseBranch() {
    NOT_IMPLEMENTED;
}

void Program::EndIfBlock() {
    NOT_IMPLEMENTED;
}


void Program::CreateWhileLoop() {
    NOT_IMPLEMENTED;
}

void Program::EndWhileLoop() {
    NOT_IMPLEMENTED;
}


void Program::CreateDoWhileLoop() {
    NOT_IMPLEMENTED;
}

void Program::BeginCondition() {
    NOT_IMPLEMENTED;
}

void Program::EndDoWhileLoop() {
    NOT_IMPLEMENTED;
}


void Program::ContinueKeyword() {
    NOT_IMPLEMENTED;
}

void Program::BreakKeyword() {
    NOT_IMPLEMENTED;
}

void Program::ReturnKeyword() {
    NOT_IMPLEMENTED;
}


void Program::ExternKeyword() {
    NOT_IMPLEMENTED;
}

void Program::StaticKeyword() {
    NOT_IMPLEMENTED;
}


void Program::CreateVariable() {
    UNDER_CONSTRUCTION;
    objects_.push_back(std::make_unique<Variable>());
}

void Program::SetName(const std::string& name) {
    UNDER_CONSTRUCTION;
    objects_.back()->SetName(name);
}

void Program::EndVariable() {
    UNDER_CONSTRUCTION;
    REQUIRE(!objects_.empty() && Is<Variable>(objects_.back().get()));
    Variable* var = As<Variable>(objects_.back().release());
    objects_.pop_back();

    var->SetType(type_stack_.back());
    type_stack_.pop_back();

    std::unique_ptr<Object> temp(var);
    if (objects_.empty()) {
        global_scope_->AddVariable(temp);
    } else {
        objects_.back()->AddVariable(temp);
    }
}


void Program::LoadVariable(const std::string& name) {
    REQUIRE(!objects_.empty() && Is<Expression>(objects_.back().get()));

    // Find its type
    Type::Ptr type;

    for (auto iter = objects_.rbegin(); iter != objects_.rend(); ++iter) {
        auto block_ptr = As<Block>(iter->get());
        if (block_ptr != nullptr) {
            auto var = block_ptr->FindVariable(name);
            if (var) {
                type = var->GetType();
                break;
            }
        }
    }

    if (!type) {
        auto var = global_scope_->FindVariable(name);
        REQUIRE(var);
        type = var->GetType();
    }

    As<Expression>(objects_.back().get())->LoadVariable(name, type);
}

void Program::PrepareToAssign() {
    NOT_IMPLEMENTED;
}

void Program::Pop() {
    NOT_IMPLEMENTED;
}

void Program::Assign() {
    NOT_IMPLEMENTED;
}


void Program::CreateStructure() {
    NOT_IMPLEMENTED;
}

void Program::EndStructure() {
    NOT_IMPLEMENTED;
}

/*
void Program::CreateTypeAlias() {
    NOT_IMPLEMENTED;
}

void Program::EndTypeAlias() {
    NOT_IMPLEMENTED;
}


void Program::FoldAsFunctionType() {
    NOT_IMPLEMENTED;
}
*/

void Program::SaveStackPos() {
    NOT_IMPLEMENTED;
}

void Program::FlushStack() {
    NOT_IMPLEMENTED;
}


void Program::Dereference() {
    NOT_IMPLEMENTED;
}


void Program::Swap() {
    NOT_IMPLEMENTED;
}

void Program::Add() {
    NOT_IMPLEMENTED;
}

void Program::Sub() {
    NOT_IMPLEMENTED;
}

void Program::Mul() {
    NOT_IMPLEMENTED;
}

void Program::Div() {
    NOT_IMPLEMENTED;
}

void Program::Mod() {
    NOT_IMPLEMENTED;
}

void Program::ShiftLeft() {
    NOT_IMPLEMENTED;
}

void Program::ShiftRight() {
    NOT_IMPLEMENTED;
}

void Program::BitwiseAnd() {
    NOT_IMPLEMENTED;
}

void Program::BitwiseOr() {
    NOT_IMPLEMENTED;
}

void Program::BitwiseXor() {
    NOT_IMPLEMENTED;
}


void Program::Dup() {
    NOT_IMPLEMENTED;
}

void Program::Inc() {
    NOT_IMPLEMENTED;
}

void Program::Dec() {
    NOT_IMPLEMENTED;
}


void Program::BeginSizeof() {
    NOT_IMPLEMENTED;
}

void Program::EndSizeof() {
    NOT_IMPLEMENTED;
}


void Program::BeginCast() {
    NOT_IMPLEMENTED;
}

void Program::EndCast() {
    NOT_IMPLEMENTED;
}


void Program::AddPointer() {
    NOT_IMPLEMENTED;
}

void Program::FindTypeByName(const std::string& name) {
    UNDER_CONSTRUCTION;
    REQUIRE(!type_stack_.empty());
    auto type_id = types_.find(name);
    REQUIRE(type_id != types_.end());

    type_stack_.back() = type_id->second->Copy();
}


void Program::LoadConstant(const std::string& value) {
    NOT_IMPLEMENTED;
}

void Program::LoadBoolConstant(const std::string& value) {
    NOT_IMPLEMENTED;
}


void Program::Subscript() {
    NOT_IMPLEMENTED;
}


void Program::PrepareToCall(const std::string& func_name) {
    NOT_IMPLEMENTED;
}

void Program::PrepareArg() {
    NOT_IMPLEMENTED;
}

void Program::Call() {
    NOT_IMPLEMENTED;
}


void Program::Member(const std::string& name) {
    NOT_IMPLEMENTED;
}

void Program::Arrow(const std::string& name) {
    NOT_IMPLEMENTED;
}


void Program::Ampersand() {
    NOT_IMPLEMENTED;
}

void Program::Asterisk() {
    NOT_IMPLEMENTED;
}

void Program::Negate() {
    NOT_IMPLEMENTED;
}

void Program::BoolNegate() {
    NOT_IMPLEMENTED;
}

void Program::BitwiseNegate() {
    NOT_IMPLEMENTED;
}


void Program::CreateOr() {
    NOT_IMPLEMENTED;
}

void Program::CheckFoldOr() {
    NOT_IMPLEMENTED;
}

void Program::Or() {
    NOT_IMPLEMENTED;
}

void Program::EndOr() {
    NOT_IMPLEMENTED;
}


void Program::CreateAnd() {
    NOT_IMPLEMENTED;
}

void Program::CheckFoldAnd() {
    NOT_IMPLEMENTED;
}

void Program::And() {
    NOT_IMPLEMENTED;
}

void Program::EndAnd() {
    NOT_IMPLEMENTED;
}


void Program::Compare() {
    NOT_IMPLEMENTED;
}

void Program::LessOrEqual() {
    NOT_IMPLEMENTED;
}

void Program::GreaterOrEqual() {
    NOT_IMPLEMENTED;
}

void Program::Less() {
    NOT_IMPLEMENTED;
}

void Program::Greater() {
    NOT_IMPLEMENTED;
}


} /* namespace vpl */
