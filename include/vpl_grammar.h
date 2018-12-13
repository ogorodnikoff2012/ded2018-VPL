#pragma once

#include "grammar_pre.h"
#include <iomanip>

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
    ENDBASICNODE()

    MAINRULE(MainRule)

    DEFRULES()
        DEFRULE(MainRule)
        BEGINRULE(MainRule)
            ASTERISK(OR(
                EXPECT(func, RULE(FunctionDefinition)),
                EXPECT(decl, RULE(DeclarationStatement))
            ));
            TOKEN(EndOfText);
        ENDRULE(MainRule)

        DEFRULE(FunctionDefinition)
        BEGINRULE(FunctionDefinition)
            EXPECT(decl, RULE(FunctionDeclaration));
            EXPECT(body, RULE(CompoundStatement));
        ENDRULE(FunctionDefinition)

        DEFRULE(CompoundStatement)
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
        BEGINRULE(ExpressionStatement)
            MAYBE(EXPECT(expr, RULE(Expression)));
            TOKEN(Semicolon);
        ENDRULE(ExpressionStatement)

        DEFRULE(IfStatement)
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
        BEGINRULE(WhileStatement)
            TOKEN(WhileKeyword);
            TOKEN(LeftParen);
            EXPECT(cond, RULE(Expression));
            TOKEN(RightParen);
            EXPECT(body, RULE(Statement));
        ENDRULE(WhileStatment)

        DEFRULE(DoWhileStatment)
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
        BEGINRULE(JumpStatement)
            OR3(
                EXPECT(continue_stmt, TOKEN(ContinueKeyword)),
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
            OR4(
                EXPECT(struct_decl, RULE(StructDeclaration)),
                EXPECT(func_decl, RULE(FunctionDeclaration)),
                EXPECT(var_decl, RULE(VariableDeclaration)),
                EXPECT(typedef_decl, RULE(TypedefDeclaration))
            );
            TOKEN(Semicolon);
        ENDRULE(DeclarationStatement)

        DEFRULE(ExternStaticSpecifier)
        BEGINRULE(ExternStaticSpecifier)
            ASTERISK(OR(
                EXPECT(extern_specifier, TOKEN(ExternKeyword)),
                EXPECT(static_specifier, TOKEN(StaticKeyword))
            ));
        ENDRULE(ExternStaticSpecifier)

        DEFRULE(VariableDeclaration)
        BEGINRULE(VariableDeclaration)
            EXPECT(spec, RULE(ExternStaticSpecifier));
            EXPECT(type, RULE(TypeName));
            EXPECT(name, TOKEN(Identifier));
            MAYBE({
                TOKEN(Assign);
                EXPECT(init, RULE(Expression));
            });
        ENDRULE(VariableDeclaration)

        DEFRULE(StructDeclaration)
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

        DEFRULE(FunctionDeclaration)
        BEGINRULE(FunctionDeclaration)
            EXPECT(spec, RULE(ExternStaticSpecifier));
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
        BEGINRULE(DeclarationArgument)
            EXPECT(type, RULE(TypeName));
            EXPECT(name, TOKEN(Identifier));
        ENDRULE(DeclarationArgument)

        DEFRULE(TypedefDeclaration)
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

        DEFRULE(Expression)
        BEGINRULE(Expression)
            EXPECT(assignment_expr, RULE(AssignmentExpression));
            ASTERISK({
                TOKEN(Comma);
                EXPECT(assignment_expr, RULE(AssignmentExpression));
            });
        ENDRULE(Expression)

        DEFRULE(AssignmentExpression)
        BEGINRULE(AssignmentExpression)
            ASTERISK({
                EXPECT(unary_expr, RULE(UnaryExpression));
                EXPECT(assign_op, RULE(AssignmentOperator));
            });
            EXPECT(cond_expr, RULE(ConditionalExpression));
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
        BEGINRULE(UnaryExpression)
            OR6(
                EXPECT(postfix_expr, RULE(PostfixExpression)),
                {
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
            );
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
            ASTERISK(EXPECT(asterisk, TOKEN(Asterisk)));
        ENDRULE(TypeName)

        DEFRULE(SimpleType)
        BEGINRULE(SimpleType)
            OR5(
                EXPECT(type, TOKEN(BoolType)),
                EXPECT(type, TOKEN(IntType)),
                EXPECT(type, TOKEN(DoubleType)),
                EXPECT(type, TOKEN(VoidType)),
                EXPECT(type, TOKEN(Identifier))
            );
        ENDRULE(SimpleType)

        DEFRULE(PostfixExpression)
        BEGINRULE(PostfixExpression)
            EXPECT(primary_expr, RULE(PrimaryExpression));
            ASTERISK(OR6(
                EXPECT(subscript_op, RULE(SubscriptOp)),
                EXPECT(call_op, RULE(CallOp)),
                EXPECT(member, RULE(Member)),
                EXPECT(arrow, RULE(Arrow)),
                EXPECT(inc_op, TOKEN(IncOp)),
                EXPECT(dec_op, TOKEN(DecOp))
            ));
        ENDRULE(PostfixExpression)

        DEFRULE(PrimaryExpression)
        BEGINRULE(PrimaryExpression)
            OR3(
                EXPECT(identifier, TOKEN(Identifier)),
                EXPECT(constant, TOKEN(Constant)),
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
        BEGINRULE(CallOp)
            TOKEN(LeftParen);
            MAYBE({
                EXPECT(arg, RULE(AssignmentExpression));
                ASTERISK({
                    TOKEN(Comma);
                    EXPECT(arg, RULE(AssignmentExpression));
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
        BEGINRULE(UnaryOperator)
            OR6(
                EXPECT(op, TOKEN(Ampresand)),
                EXPECT(op, TOKEN(Asterisk)),
                EXPECT(op, TOKEN(Plus)),
                EXPECT(op, TOKEN(Minus)),
                EXPECT(op, TOKEN(Not)),
                EXPECT(op, TOKEN(BitwiseNot))
            );
        ENDRULE(UnaryOperator)

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

        DEFRULE(OrExpression)
        BEGINRULE(OrExpression)
            EXPECT(and_expr, RULE(AndExpression));
            ASTERISK({
                TOKEN(Or);
                EXPECT(and_expr, RULE(AndExpression));
            });
        ENDRULE(OrExpression)

        DEFRULE(AndExpression)
        BEGINRULE(AndExpression)
            EXPECT(bitwise_or_expr, RULE(BitwiseOrExpression));
            ASTERISK({
                TOKEN(And);
                EXPECT(bitwise_or_expr, RULE(BitwiseOrExpression));
            });
        ENDRULE(AndExpression)

        DEFRULE(BitwiseOrExpression)
        BEGINRULE(BitwiseOrExpression)
            EXPECT(bitwise_xor_expr, RULE(BitwiseXorExpression));
            ASTERISK({
                TOKEN(BitwiseOr);
                EXPECT(bitwise_xor_expr, RULE(BitwiseXorExpression));
            });
        ENDRULE(BitwiseOrExpression)

        DEFRULE(BitwiseXorExpression)
        BEGINRULE(BitwiseXorExpression)
            EXPECT(bitwise_and_expr, RULE(BitwiseAndExpression));
            ASTERISK({
                TOKEN(Xor);
                EXPECT(bitwise_and_expr, RULE(BitwiseAndExpression));
            });
        ENDRULE(BitwiseXorExpression)

        DEFRULE(BitwiseAndExpression)
        BEGINRULE(BitwiseAndExpression)
            EXPECT(equality_expr, RULE(EqualityExpression));
            ASTERISK({
                TOKEN(BitwiseOr);
                EXPECT(equality_expr, RULE(EqualityExpression));
            });
        ENDRULE(BitwiseAndExpression)

        DEFRULE(EqualityExpression)
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
        BEGINRULE(MultiplicativeExpression)
            EXPECT(cast_expr, RULE(CastExpression));
            ASTERISK({
                OR3(
                    EXPECT(op, TOKEN(Multiply)),
                    EXPECT(op, TOKEN(Divide)),
                    EXPECT(op, TOKEN(Modulo))
                );
                EXPECT(cast_expr, RULE(CastExpression));
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

        DEFTOKEN(Ampresand, "&")
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
