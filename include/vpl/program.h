#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "object.h"
#include "type.h"
#include "function_declaration.h"
#include "function.h"

namespace vpl {

class Program {
public:
    Program()
        : types_{
            {"int", std::make_shared<Type>(Type::kTypeInt, "int")},
//          {"double", std::make_shared<Type>(Type::kTypeDouble, "double")},
            {"bool", std::make_shared<Type>(Type::kTypeBool, "bool")},
        } {
        global_scope_ = std::make_shared<Block>();
    }

    ~Program();

    void CreateFunction();
    void EndFunction();

    void DeclareFunction();
    void EndFunctionDeclaration();

    void BeginBody();
    void BeginType();

    void CreateBlock();
    void EndBlock();

    void CreateExpression();
    void EndExpression();

    void CreateIfBlock();
    void BeginTrueBranch();
    void BeginFalseBranch();
    void EndIfBlock();

    void CreateWhileLoop();
    void EndWhileLoop();

    void CreateDoWhileLoop();
    void BeginCondition();
    void EndDoWhileLoop();

    void ContinueKeyword();
    void BreakKeyword();
    void ReturnKeyword();

    void ExternKeyword();
    void StaticKeyword();

    void CreateVariable();
    void SetName(const std::string& name);
    void EndVariable();

    void LoadVariable(const std::string& name);
    void PrepareToAssign();
    void Pop();
    void Assign();

    void CreateStructure();
    void EndStructure();

//    void CreateTypeAlias();
//    void EndTypeAlias();

//    void FoldAsFunctionType();

    void SaveStackPos();
    void FlushStack();

    void Dereference();

    void Swap();
    void Add();
    void Sub();
    void Mul();
    void Div();
    void Mod();
    void ShiftLeft();
    void ShiftRight();
    void BitwiseAnd();
    void BitwiseOr();
    void BitwiseXor();

    void Dup();
    void Inc();
    void Dec();

    void BeginSizeof();
    void EndSizeof();

    void BeginCast();
    void EndCast();

    void AddPointer();
    void FindTypeByName(const std::string& name);

    void LoadConstant(const std::string& value);
    void LoadBoolConstant(const std::string& value);

    void Subscript();

    void PrepareToCall(const std::string& func_name);
    void PrepareArg();
    void Call();

    void Member(const std::string& name);
    void Arrow(const std::string& name);

    void Ampersand();
    void Asterisk();
    void Negate();
    void BoolNegate();
    void BitwiseNegate();

    void CreateOr();
    void CheckFoldOr();
    void Or();
    void EndOr();

    void CreateAnd();
    void CheckFoldAnd();
    void And();
    void EndAnd();

    void Compare();
    void LessOrEqual();
    void GreaterOrEqual();
    void Less();
    void Greater();

    std::vector<Type::Ptr>& GetTypeStack() {
        return type_stack_;
    }

    void AddFunctionDeclaration(FunctionDeclaration* func_decl);

private:
    std::vector<Object::Ptr> objects_;
    std::unordered_map<std::string, FunctionDeclaration::Ptr> function_declarations_;
    std::unordered_map<std::string, Type::Ptr> types_;
    std::vector<Type::Ptr> type_stack_;
    std::vector<Function::Ptr> functions_;
    Block::Ptr global_scope_;
};

}  /* namespace vpl */
