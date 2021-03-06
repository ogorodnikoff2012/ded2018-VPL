#pragma once

#include "grammar_pre.h"
#include <iomanip>
#include <memory>

#include "vpl/scope.h"

#define GET_TOKEN_STRING(node) dynamic_cast<const ASTTokenBasic*>(( node ).get())->GetStr()

DEFGRAMMAR(VPL)
    DEFBASICNODE()
    public:
        virtual void Print(std::ostream& out) {
            out << '[' << std::quoted(GetName()) << ", ";
            bool is_first = true;
            for (const auto& node : children_) {
                if (is_first) {
                    is_first = false;
                } else {
                    out << ", ";
                }
                node->Print(out);
            }
            out << ']';
        }
        virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const {
            for (const auto& child : children_) {
                child->BuildProgram(scope, out);
            }
        }
    ENDBASICNODE()

    MAINRULE(MainRule)

    DEFRULES()
        DEFRULE(MainRule)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                for (size_t i = 0; i < children_.size(); ++i) {
                    children_[i]->BuildProgram(scope, out);
                }
                scope->Finish();
            }
        BEGINRULE(MainRule)
            ASTERISK(OR(
                EXPECT(func, RULE(FunctionDefinition)),
                EXPECT(decl, RULE(DeclarationStatement))
            ));
            TOKEN(EndOfText);
        ENDRULE(MainRule)

        DEFRULE(FunctionDefinition)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                out << "func " << scope->GetLastFunctionName() << '\n';
                scope->SpawnFunctionScope();
                children_[1]->BuildProgram(scope, out);
                scope->CloseScope();
                out << "    ret\n";
            }
        BEGINRULE(FunctionDefinition)
            EXPECT(decl, RULE(FunctionDeclaration));
            EXPECT(body, RULE(CompoundStatement));
        ENDRULE(FunctionDefinition)

        DEFRULE(CompoundStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->OpenScope();
                for (const auto& child : children_) {
                    child->BuildProgram(scope, out);
                }
                scope->CloseScope();
            }
        BEGINRULE(CompoundStatement)
            TOKEN(LeftBrace);
            ASTERISK(EXPECT(stmt, RULE(Statement)));
            TOKEN(RightBrace);
        ENDRULE(CompoundStatement)

        DEFRULE(Statement)
        BEGINRULE(Statement)
            OR6(
                EXPECT(stmt, RULE(CompoundStatement)),
                EXPECT(stmt, RULE(ExpressionStatement)),
                EXPECT(stmt, RULE(IfStatement)),
                EXPECT(stmt, RULE(LoopStatement)),
                EXPECT(stmt, RULE(JumpStatement)),
                EXPECT(stmt, RULE(DeclarationStatement));
            );
        ENDRULE(Statement)

        DEFRULE(ExpressionStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                if (!children_.empty()) {
                    scope->SaveStackPos();
                    children_[0]->BuildProgram(scope, out);
                    scope->RestoreStackPos();
                }
            }
        BEGINRULE(ExpressionStatement)
            MAYBE(EXPECT(expr, RULE(Expression)));
            TOKEN(Semicolon);
        ENDRULE(ExpressionStatement)

        DEFRULE(IfStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                int unique_id = scope->GetUnique();

                out << "func .L" << unique_id << "_if\n";
                children_[0]->BuildProgram(scope, out);
                out << "    jz .L" << unique_id << "_else\n";

                out << "func .L" << unique_id << "_then\n";
                out << "    pop 0\n";
                children_[1]->BuildProgram(scope, out);
                out << "    jmp .L" << unique_id << "_end\n";

                out << "func .L" << unique_id << "_else\n";
                out << "    pop 0\n";
                if (children_.size() == 3) {
                    children_[2]->BuildProgram(scope, out);
                }

                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(IfStatement)
            TOKEN(IfKeyword);
            TOKEN(LeftParen);
            EXPECT(cond, RULE(Expression));
            TOKEN(RightParen);
            EXPECT(true_branch, RULE(Statement));
            MAYBE({
                TOKEN(ElseKeyword);
                EXPECT(false_branch, RULE(Statement));
            });
        ENDRULE(IfStatement)

        DEFRULE(LoopStatement)
        BEGINRULE(LoopStatement)
            OR3(
                EXPECT(while_loop, RULE(WhileStatement)),
                EXPECT(do_while_loop, RULE(DoWhileStatment)),
                EXPECT(for_loop, RULE(ForStatement))
            );
        ENDRULE(LoopStatement)

        DEFRULE(WhileStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                int unique_id = scope->GetUnique();

                out << "func .L" << unique_id << "_while\n";
                children_[0]->BuildProgram(scope, out);
                out << "    jz .L" << unique_id << "_end_while\n";

                out << "    pop 0\n";
                children_[1]->BuildProgram(scope, out);
                out << "    jmp .L" << unique_id << "_while\n";
                out << "func .L" << unique_id << "_end_while\n";
                out << "    pop 0\n";
                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(WhileStatement)
            TOKEN(WhileKeyword);
            TOKEN(LeftParen);
            EXPECT(cond, RULE(Expression));
            TOKEN(RightParen);
            EXPECT(body, RULE(Statement));
        ENDRULE(WhileStatment)

        DEFRULE(DoWhileStatment)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                int unique_id = scope->GetUnique();
                out << "    push 0\n";
                out << "func .L" << unique_id << "_do_while\n";
                out << "    pop 0\n";
                children_[0]->BuildProgram(scope, out);
                children_[1]->BuildProgram(scope, out);
                out << "    jnz .L" << unique_id << "_do_while\n";
                out << "    pop 0\n";
                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(DoWhileStatment)
            TOKEN(DoKeyword);
            EXPECT(body, RULE(Statement));
            TOKEN(WhileKeyword);
            TOKEN(LeftParen);
            EXPECT(cond, RULE(Expression));
            TOKEN(RightParen);
            TOKEN(Semicolon);
        ENDRULE(DoWhileStatment)

        DEFRULE(ForStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                int unique_id = scope->GetUnique();
                children_[0]->BuildProgram(scope, out);
                out << "func .L" << unique_id << "_for\n";
                children_[1]->BuildProgram(scope, out);
                out << "    jz .L" << unique_id << "_end_for\n";
                out << "    pop 0\n";
                children_[3]->BuildProgram(scope, out);
                children_[2]->BuildProgram(scope, out);
                out << "    jmp .L" << unique_id << "_for\n";
                out << "func .L" << unique_id << "_end_for\n";
                out << "    pop 0\n";
                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(ForStatement)
            TOKEN(ForKeyword);
            TOKEN(LeftParen);
            EXPECT(init, RULE(ExpressionStatement));
            EXPECT(cond, RULE(ExpressionStatement));
            MAYBE(EXPECT(iter_expr, RULE(Expression)));
            TOKEN(RightParen);
            EXPECT(body, RULE(Statement));
        ENDRULE(ForStatement)

        DEFRULE(JumpStatement)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                if (children_[0]->GetType() == kBreakKeywordType) {
                    out << "    jmp .L" << scope->GetPreviousUnique() << "_end\n";
                } else {
                    if (children_.size() == 2) {
                        children_[1]->BuildProgram(scope, out);
                    }
                    if (children_.size() == 2) {
                        out << "    pop %" << scope->RetRegister() << '\n';
                    }
                    out << "    ret\n";
                }
            }
        BEGINRULE(JumpStatement)
            OR(
//                EXPECT(continue_stmt, TOKEN(ContinueKeyword)),
                EXPECT(break_stmt, TOKEN(BreakKeyword)),
                {
                    EXPECT(return_stmt, TOKEN(ReturnKeyword));
                    MAYBE(EXPECT(expr, RULE(Expression)));
                }
            );
            TOKEN(Semicolon);
        ENDRULE(JumpStatement)

        DEFRULE(DeclarationStatement)
        BEGINRULE(DeclarationStatement)
            OR(
//                EXPECT(struct_decl, RULE(StructDeclaration)),
//                EXPECT(typedef_decl, RULE(TypedefDeclaration)),
                EXPECT(func_decl, RULE(FunctionDeclaration)),
                EXPECT(var_decl, RULE(VariableDeclaration))
            );
            TOKEN(Semicolon);
        ENDRULE(DeclarationStatement)
/*
        DEFRULE(ExternStaticSpecifier)
            virtual void BuildProgram(vpl::Program* program) const override {
                for (const auto& child : children_) {
                    if (child->GetType() == kExternKeywordType) {
                        program->ExternKeyword();
                    } else {
                        program->StaticKeyword();
                    }
                }
            }
        BEGINRULE(ExternStaticSpecifier)
            ASTERISK(OR(
                EXPECT(extern_specifier, TOKEN(ExternKeyword)),
                EXPECT(static_specifier, TOKEN(StaticKeyword))
            ));
        ENDRULE(ExternStaticSpecifier)
*/
        DEFRULE(VariableDeclaration)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->CreateVariable(GET_TOKEN_STRING(children_[0]));
            }
        BEGINRULE(VariableDeclaration)
 //           EXPECT(spec, RULE(ExternStaticSpecifier));
//            EXPECT(type, RULE(TypeName));
            TOKEN(IntType);
            EXPECT(name, TOKEN(Identifier));
/*            MAYBE({
                TOKEN(Assign);
                EXPECT(init, RULE(OrExpression));
            });*/
        ENDRULE(VariableDeclaration)

    /*
        DEFRULE(StructDeclaration)
            virtual void BuildProgram(vpl::Program* program) const override {
                program->CreateStructure();
                program->SetName(GET_TOKEN_STRING(children_[0]));
                for (size_t i = 1; i < children_.size(); i += 2) {
                    program->BeginType();
                    children_[i]->BuildProgram(program);
                    program->SetName(GET_TOKEN_STRING(children_[i + 1]));
                }
                program->EndStructure();
            }
        BEGINRULE(StructDeclaration)
            TOKEN(StructKeyword);
            EXPECT(name, TOKEN(Identifier));
            TOKEN(LeftBrace);
            ASTERISK({
                EXPECT(type, RULE(TypeName));
                EXPECT(name, TOKEN(Identifier));
                TOKEN(Semicolon);
            });
            TOKEN(RightBrace);
        ENDRULE(StructDeclaration)
*/
        DEFRULE(FunctionDeclaration)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->DeclareFunction(GET_TOKEN_STRING(children_[1]));
                children_[2]->BuildProgram(scope, out);
                scope->EndFunctionDeclaration();
            }
        BEGINRULE(FunctionDeclaration)
//            EXPECT(spec, RULE(ExternStaticSpecifier));
            EXPECT(type, RULE(TypeName));
            EXPECT(name, TOKEN(Identifier));

            TOKEN(LeftParen);
            EXPECT(arg_list, RULE(DeclarationArgumentList));
            TOKEN(RightParen);
        ENDRULE(FunctionDeclaration)

        DEFRULE(DeclarationArgumentList)
        BEGINRULE(DeclarationArgumentList)
            MAYBE({
                EXPECT(arg, RULE(DeclarationArgument));
                ASTERISK({
                    TOKEN(Comma);
                    EXPECT(arg, RULE(DeclarationArgument));
                });
            });
        ENDRULE(DeclarationArgumentList)

        DEFRULE(DeclarationArgument)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->AddArgumentName(GET_TOKEN_STRING(children_[1]));
            }
        BEGINRULE(DeclarationArgument)
            EXPECT(type, RULE(TypeName));
            EXPECT(name, TOKEN(Identifier));
        ENDRULE(DeclarationArgument)

/*        DEFRULE(TypedefDeclaration)
            virtual void BuildProgram(vpl::Program* program) const override {
                program->CreateTypeAlias();
                if (children_.size() == 2) {
                    program->BeginType();
                    children_[0]->BuildProgram(program);
                    program->SetName(GET_TOKEN_STRING(children_[1]));
                } else {
                    children_[0]->BuildProgram(program);
                }
                program->EndTypeAlias();
            }
        BEGINRULE(TypedefDeclaration)
            TOKEN(TypedefKeyword);
            OR(
                {
                    EXPECT(type, RULE(TypeName));
                    EXPECT(name, TOKEN(Identifier));
                },
                EXPECT(func_ptr_typedef, RULE(FunctionPointerTypedef))
            );
        ENDRULE(TypedefDeclaration)


        DEFRULE(FunctionPointerTypedef)
            virtual void BuildProgram(vpl::Program* program) const override {
                program->BeginType();
                children_[0]->BuildProgram(program);
                for (size_t i = 2; i < children_.size(); ++i) {
                    program->BeginType();
                    children_[i]->BuildProgram(program);
                }
                program->FoldAsFunctionType();
                program->SetName(GET_TOKEN_STRING(children_[1]));
            }
        BEGINRULE(FunctionPointerTypedef)
            EXPECT(return_type, RULE(TypeName));
            TOKEN(LeftParen);
            TOKEN(Asterisk);
            EXPECT(name, TOKEN(Identifier));
            TOKEN(RightParen);
            TOKEN(LeftParen);
            MAYBE({
                EXPECT(type, RULE(TypeName));
                ASTERISK({
                    TOKEN(Comma);
                    EXPECT(type, RULE(TypeName));
                });
            });
            TOKEN(RightParen);
        ENDRULE(FunctionPointerTypedef)
*/
        DEFRULE(Expression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 1; i < children_.size(); ++i) {
                    scope->SaveStackPos();
                    scope->RestoreStackPos();
                    children_[i]->BuildProgram(scope, out);
                }
            }
        BEGINRULE(Expression)
            EXPECT(assignment_expr, RULE(AssignmentExpression));
            ASTERISK({
                TOKEN(Comma);
                EXPECT(assignment_expr, RULE(AssignmentExpression));
            });
        ENDRULE(Expression)

        DEFRULE(AssignmentExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                if (children_.size() == 1) {
                    children_[0]->BuildProgram(scope, out);
                    return;
                }
                children_[1]->BuildProgram(scope, out);
                scope->LoadVariableAddr(GET_TOKEN_STRING(children_[0]));
                out << "    pop %" << scope->AssignRegister() << '\n';
                scope->Pop();
                out << "    dup\n";
                out << "    pop !" << scope->AssignRegister() << '\n';
            }
        BEGINRULE(AssignmentExpression)
            MAYBE({
                EXPECT(name, TOKEN(Identifier));
                TOKEN(Assign);
            });
            EXPECT(cond_expr, RULE(OrExpression));
        ENDRULE(AssignmentExpression)

        DEFRULE(AssignmentOperator)
        BEGINRULE(AssignmentOperator)
            OR6(
                EXPECT(op, TOKEN(Assign)),
                EXPECT(op, TOKEN(AddAssign)),
                EXPECT(op, TOKEN(SubAssign)),
                EXPECT(op, TOKEN(MulAssign)),
                EXPECT(op, TOKEN(DivAssign)),
            OR6(
                EXPECT(op, TOKEN(ModAssign)),
                EXPECT(op, TOKEN(ShiftLeftAssign)),
                EXPECT(op, TOKEN(ShiftRightAssign)),
                EXPECT(op, TOKEN(AndAssign)),
                EXPECT(op, TOKEN(OrAssign)),
                EXPECT(op, TOKEN(XorAssign))
            ));
        ENDRULE(AssignmentOperator)

        DEFRULE(UnaryExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                for (int i = children_.size() - 1; i >= 0; --i) {
                    children_[i]->BuildProgram(scope, out);
                }
            }
        BEGINRULE(UnaryExpression)
//            OR5(
            ASTERISK(EXPECT(op, RULE(UnaryOperator)));
            EXPECT(postfix_expr, RULE(PostfixExpression));
/*                {
                    EXPECT(inc_op, TOKEN(IncOp));
                    EXPECT(unary_expr, RULE(UnaryExpression));
                },
                {
                    EXPECT(dec_op, TOKEN(DecOp));
                    EXPECT(unary_expr, RULE(UnaryExpression));
                },
                {
                    EXPECT(unary_op, RULE(UnaryOperator));
                    EXPECT(cast_expr, RULE(CastExpression));
                },
                {
                    EXPECT(sizeof_op, TOKEN(SizeofKeyword));
                    EXPECT(unary_expr, RULE(UnaryExpression));
                },
                {
                    EXPECT(sizeof_op, TOKEN(SizeofKeyword));
                    TOKEN(LeftParen);
                    EXPECT(type, RULE(TypeName));
                    TOKEN(RightParen);
                }
            );*/
        ENDRULE(UnaryExpression)

        DEFRULE(CastExpression)
        BEGINRULE(CastExpression)
            OR(
                {
                    TOKEN(LeftParen);
                    EXPECT(type, RULE(TypeName));
                    TOKEN(RightParen);
                    EXPECT(cast_expr, RULE(CastExpression));
                },
                EXPECT(unary_expr, RULE(UnaryExpression))
            );
        ENDRULE(CastExpression)


        DEFRULE(TypeName)
        BEGINRULE(TypeName)
            EXPECT(simple_type, RULE(SimpleType));
        //    ASTERISK(EXPECT(asterisk, TOKEN(Asterisk)));
        ENDRULE(TypeName)

        DEFRULE(SimpleType)
        BEGINRULE(SimpleType)
            TOKEN(IntType);
/*
            OR5(
                EXPECT(type, TOKEN(BoolType)),
                EXPECT(type, TOKEN(IntType)),
                EXPECT(type, TOKEN(DoubleType)),
                EXPECT(type, TOKEN(VoidType)),
                EXPECT(type, TOKEN(Identifier))
            );
            */
        ENDRULE(SimpleType)

        DEFRULE(PostfixExpression)
        BEGINRULE(PostfixExpression)
            OR(
                EXPECT(call_expr, RULE(CallExpression)),
                EXPECT(primary_expr, RULE(PrimaryExpression))
            );
/*
            ASTERISK(OR5(
                EXPECT(subscript_op, RULE(SubscriptOp)),
//                EXPECT(call_op, RULE(CallOp)),
                EXPECT(member, RULE(Member)),
                EXPECT(arrow, RULE(Arrow)),
                EXPECT(inc_op, TOKEN(IncOp)),
                EXPECT(dec_op, TOKEN(DecOp))
            ));
            */
        ENDRULE(PostfixExpression)

        DEFRULE(CallExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->PrepareFunction(GET_TOKEN_STRING(children_[0]));
                children_[1]->BuildProgram(scope, out);
                scope->CallFunction(GET_TOKEN_STRING(children_[0]));
            }
        BEGINRULE(CallExpression)
            EXPECT(func_name, TOKEN(Identifier));
            EXPECT(call_op, RULE(CallOp));
        ENDRULE(CallExpression)

        DEFRULE(PrimaryExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                switch (children_[0]->GetType()) {
                    case kConstantType:
                    case kBoolConstantType:
                        out << "    push " << GET_TOKEN_STRING(children_[0]) << '\n';
                        scope->Push();
                        break;
                    case kIdentifierType:
                        scope->LoadVariable(GET_TOKEN_STRING(children_[0]));
                        break;
                    default:
                        children_[0]->BuildProgram(scope, out);
                        break;
                }
            }
        BEGINRULE(PrimaryExpression)
            OR4(
                EXPECT(constant, TOKEN(Constant)),
                EXPECT(bool_constant, TOKEN(BoolConstant)),
                EXPECT(identifier, TOKEN(Identifier)),
                {
                    TOKEN(LeftParen);
                    EXPECT(expr, RULE(Expression));
                    TOKEN(RightParen);
                }
            );
        ENDRULE(PrimaryExpression)

        DEFRULE(SubscriptOp)
        BEGINRULE(SubscriptOp)
            TOKEN(LeftBracket);
            EXPECT(expr, RULE(Expression));
            TOKEN(RightBracket);
        ENDRULE(SubscriptOp)

        DEFRULE(CallOp)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                scope->CheckArgCnt(children_.size());
                for (int i = children_.size() - 1; i >= 0; --i) {
                    children_[i]->BuildProgram(scope, out);
                }
            }
        BEGINRULE(CallOp)
            TOKEN(LeftParen);
            MAYBE({
                EXPECT(arg, RULE(OrExpression));
                ASTERISK({
                    TOKEN(Comma);
                    EXPECT(arg, RULE(OrExpression));
                });
            });
            TOKEN(RightParen);
        ENDRULE(CallOp)

        DEFRULE(Member)
        BEGINRULE(Member)
            TOKEN(MemberOp);
            EXPECT(member, TOKEN(Identifier));
        ENDRULE(Member)

        DEFRULE(Arrow)
        BEGINRULE(Arrow)
            TOKEN(ArrowOp);
            EXPECT(member, TOKEN(Identifier));
        ENDRULE(Arrow)

        DEFRULE(UnaryOperator)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                switch (children_[0]->GetType()) {
                    case kMinusType:
                        out << "    neg\n";
                        break;
                    case kNotType:
                        out << "    not\n";
                        break;
                    case kBitwiseNotType:
                        out << "    push -1\n";
                        out << "    xor\n";
                        break;
                }
            }
        BEGINRULE(UnaryOperator)
            OR4(
//                EXPECT(op, TOKEN(Ampersand)),
//                EXPECT(op, TOKEN(Asterisk)),
                EXPECT(op, TOKEN(Plus)),
                EXPECT(op, TOKEN(Minus)),
                EXPECT(op, TOKEN(Not)),
                EXPECT(op, TOKEN(BitwiseNot))
            );
        ENDRULE(UnaryOperator)

/*
        DEFRULE(ConditionalExpression)
        BEGINRULE(ConditionalExpression)
            EXPECT(or_expr, RULE(OrExpression));
            MAYBE({
                TOKEN(QuestionMark);
                EXPECT(expr, RULE(Expression));
                TOKEN(Colon);
                EXPECT(cond_expr, RULE(ConditionalExpression));
            });
        ENDRULE(ConditionalExpression)
*/
        DEFRULE(OrExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                if (children_.size() == 1) {
                    return;
                }
                out << "    bool\n";
                int unique_id = scope->GetUnique();
                for (size_t i = 1; i < children_.size(); ++i) {
                    out << "    jnz .L" << unique_id << "_end\n";
                    children_[i]->BuildProgram(scope, out);
                    out << "    bool\n";
                    out << "    or\n";
                    scope->Pop();
                }
                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(OrExpression)
            EXPECT(and_expr, RULE(AndExpression));
            ASTERISK({
                TOKEN(Or);
                EXPECT(and_expr, RULE(AndExpression));
            });
        ENDRULE(OrExpression)

        DEFRULE(AndExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                if (children_.size() == 1) {
                    return;
                }
                out << "    bool\n";
                int unique_id = scope->GetUnique();
                for (size_t i = 1; i < children_.size(); ++i) {
                    out << "    jz .L" << unique_id << "_end\n";
                    children_[i]->BuildProgram(scope, out);
                    out << "    bool\n";
                    out << "    and\n";
                    scope->Pop();
                }
                out << "func .L" << unique_id << "_end\n";
            }
        BEGINRULE(AndExpression)
            EXPECT(bitwise_or_expr, RULE(BitwiseOrExpression));
            ASTERISK({
                TOKEN(And);
                EXPECT(bitwise_or_expr, RULE(BitwiseOrExpression));
            });
        ENDRULE(AndExpression)

        DEFRULE(BitwiseOrExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 1; i < children_.size(); ++i) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    or\n";
                    scope->Pop();
                }
            }
        BEGINRULE(BitwiseOrExpression)
            EXPECT(bitwise_xor_expr, RULE(BitwiseXorExpression));
            ASTERISK({
                TOKEN(BitwiseOr);
                EXPECT(bitwise_xor_expr, RULE(BitwiseXorExpression));
            });
        ENDRULE(BitwiseOrExpression)

        DEFRULE(BitwiseXorExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 1; i < children_.size(); ++i) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    xor\n";
                    scope->Pop();
                }
            }
        BEGINRULE(BitwiseXorExpression)
            EXPECT(bitwise_and_expr, RULE(BitwiseAndExpression));
            ASTERISK({
                TOKEN(Xor);
                EXPECT(bitwise_and_expr, RULE(BitwiseAndExpression));
            });
        ENDRULE(BitwiseXorExpression)

        DEFRULE(BitwiseAndExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 1; i < children_.size(); ++i) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    and\n";
                    scope->Pop();
                }
            }
        BEGINRULE(BitwiseAndExpression)
            EXPECT(equality_expr, RULE(EqualityExpression));
            ASTERISK({
                TOKEN(BitwiseOr);
                EXPECT(equality_expr, RULE(EqualityExpression));
            });
        ENDRULE(BitwiseAndExpression)

        DEFRULE(EqualityExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 2; i < children_.size(); i += 2) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    " << (children_[i - 1]->GetType() == kEqualOpType ? "ceq" : "cne") << "\n";
                    scope->Pop();
                }
            }
        BEGINRULE(EqualityExpression)
            EXPECT(rel_expr, RULE(RelationalExpression));
            ASTERISK({
                OR(
                    EXPECT(eq_op, TOKEN(EqualOp)),
                    EXPECT(ne_op, TOKEN(NotEqualOp))
                );
                EXPECT(rel_expr, RULE(RelationalExpression));
            });
        ENDRULE(EqualityExpression)

        DEFRULE(RelationalExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 2; i < children_.size(); i += 2) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    ";
                    switch(children_[i - 1]->GetType()) {
                        case kLessOrEqualOpType:
                            out << "    cle\n";
                            break;
                        case kGreaterOrEqualOpType:
                            out << "    cge\n";
                            break;
                        case kLessOpType:
                            out << "    clt\n";
                            break;
                        case kGreaterOpType:
                            out << "    cgt\n";
                            break;
                    }
                    out << '\n';
                    scope->Pop();
                }
            }
        BEGINRULE(RelationalExpression)
            EXPECT(shift_expr, RULE(ShiftExpression));
            ASTERISK({
                OR4(
                    EXPECT(op, TOKEN(LessOrEqualOp)),
                    EXPECT(op, TOKEN(GreaterOrEqualOp)),
                    EXPECT(op, TOKEN(LessOp)),
                    EXPECT(op, TOKEN(GreaterOp))
                );
                EXPECT(shift_expr, RULE(ShiftExpression));
            });
        ENDRULE(RelationalExpression)

        DEFRULE(ShiftExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 2; i < children_.size(); i += 2) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    ";
                    switch(children_[i - 1]->GetType()) {
                        case kShiftLeftType:
                            out << "    shl\n";
                            break;
                        case kShiftRightType:
                            out << "    shr\n";
                            break;
                    }
                    out << '\n';
                    scope->Pop();
                }
            }
        BEGINRULE(ShiftExpression)
            EXPECT(add_expr, RULE(AdditiveExpression));
            ASTERISK({
                OR(
                    EXPECT(op, TOKEN(ShiftLeft)),
                    EXPECT(op, TOKEN(ShiftRight))
                );
                EXPECT(add_expr, RULE(AdditiveExpression));
            });
        ENDRULE(ShiftExpression)

        DEFRULE(AdditiveExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 2; i < children_.size(); i += 2) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    ";
                    switch(children_[i - 1]->GetType()) {
                        case kPlusType:
                            out << "    add\n";
                            break;
                        case kMinusType:
                            out << "    sub\n";
                            break;
                    }
                    out << '\n';
                    scope->Pop();
                }
            }
        BEGINRULE(AdditiveExpression)
            EXPECT(mul_expr, RULE(MultiplicativeExpression))
            ASTERISK({
                OR(
                    EXPECT(op, TOKEN(Plus)),
                    EXPECT(op, TOKEN(Minus))
                );
                EXPECT(mul_expr, RULE(MultiplicativeExpression));
            });
        ENDRULE(AdditiveExpression)

        DEFRULE(MultiplicativeExpression)
            virtual void BuildProgram(vpl::Scope* scope, std::ostream& out) const override {
                children_[0]->BuildProgram(scope, out);
                for (size_t i = 2; i < children_.size(); i += 2) {
                    children_[i]->BuildProgram(scope, out);
                    out << "    ";
                    switch(children_[i - 1]->GetType()) {
                        case kMultiplyType:
                            out << "    mul\n";
                            break;
                        case kDivideType:
                            out << "    div\n";
                            break;
                        case kModuloType:
                            out << "    mod\n";
                            break;
                    }
                    out << '\n';
                    scope->Pop();
                }
            }
        BEGINRULE(MultiplicativeExpression)
            EXPECT(cast_expr, RULE(CastExpression));
            ASTERISK({
                OR3(
                    EXPECT(op, TOKEN(Multiply)),
                    EXPECT(op, TOKEN(Divide)),
                    EXPECT(op, TOKEN(Modulo))
                );
//                EXPECT(cast_expr, RULE(CastExpression));
                EXPECT(unary_expr, RULE(UnaryExpression));
            });
        ENDRULE(MultiplicativeExpression)

    ENDRULES()

#define TOKEN_PRINT virtual void Print(std::ostream& out) override {            \
    out << '[' << std::quoted(GetName()) << ", " << std::quoted(str_) << ']';   \
}

#define UNEXTENDIBLE ")([^[:alnum:]_]|$"

    DEFTOKENS()
        DEFTOKEN(LeftParen, "\\(")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(RightParen, "\\)")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Comma, ",")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Identifier, "[[:alpha:]_][[:alnum:]_]*")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Constant, "([0-9]+(\\.[0-9]*)?|[0-9]*\\.[0-9]+)([eE][-\\+]?[0-9]+)?")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BoolConstant, "true|false")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Plus, "\\+")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Minus, "-")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Multiply, "\\*")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Divide, "/")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Modulo, "%")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ShiftLeft, "<<")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ShiftRight, ">>")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Or, "\\|\\|")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BitwiseOr, "\\|")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(And, "&&")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BitwiseAnd, "&")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Xor, "\\^")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Not, "!")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BitwiseNot, "~")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ArrowOp, "->")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(MemberOp, "\\.")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(LeftBracket, "\\[")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(RightBracket, "\\]")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(LeftBrace, "\\{")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(RightBrace, "\\}")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Semicolon, ";")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Ampersand, "&")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Asterisk, "\\*")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(IncOp, "\\+\\+")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(DecOp, "--")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(QuestionMark, "\\?")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Colon, ":")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(EqualOp, "==")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(NotEqualOp, "!=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(LessOp, "<")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(GreaterOp, ">")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(LessOrEqualOp, "<=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(GreaterOrEqualOp, ">=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(StaticKeyword, "static" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ExternKeyword, "extern" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(TypedefKeyword, "typedef" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(SizeofKeyword, "sizeof" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(EndOfText, "$")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(VoidType, "void" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(IntType, "int" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BoolType, "bool" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(TrueKeyword, "true" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(FalseKeyword, "false" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(DoubleType, "double" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(StructKeyword, "struct" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(IfKeyword, "if" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ElseKeyword, "else" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ForKeyword, "for" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(WhileKeyword, "while" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(DoKeyword, "do" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ContinueKeyword, "continue" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(BreakKeyword, "break" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ReturnKeyword, "return" UNEXTENDIBLE)
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(Assign, "=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(AddAssign, "\\+=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(SubAssign, "-=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(MulAssign, "\\*=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(DivAssign, "/=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ModAssign, "%=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ShiftLeftAssign, "<<=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(ShiftRightAssign, ">>=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(AndAssign, "&=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(OrAssign, "\\|=")
            TOKEN_PRINT
        ENDTOKEN()

        DEFTOKEN(XorAssign, "\\^=")
            TOKEN_PRINT
        ENDTOKEN()
    ENDTOKENS()
#undef TOKEN_PRINT
ENDGRAMMAR(VPL)

#include "grammar_post.h"
