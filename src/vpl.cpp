#include <vpl_grammar.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    std::string expression;

    if (argc > 1 && std::strcmp(argv[1], "-") != 0) {
        std::ifstream fin(argv[1]);
        expression.assign((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));
    } else {
        std::getline(std::cin, expression);
    }
    std::cerr << "Input:\n" << expression << std::endl;

    std::string out_fname = argv[1];
    out_fname += ".vasm";
    std::ofstream asm_out(out_fname);

    VPLGrammar::Parser parser;
    try {
        auto expr = parser.Parse(expression);
        expr->Print(std::cout);
        std::cout << std::endl;

        vpl::Scope scope(asm_out);
        expr->BuildProgram(&scope, asm_out);
    } catch (std::exception& ex) {
        std::cout << ex.what();
    }
    std::cout << std::endl;
    return 0;
}
