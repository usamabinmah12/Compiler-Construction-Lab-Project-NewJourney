#include "ast/ast.hpp"
#include <iostream>

extern int yyparse();
extern FILE *yyin;
extern ASTNode *program_root;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./compiler <input.nj>" << std::endl;
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Error: cannot open " << argv[1] << std::endl;
        return 1;
    }
    int result = yyparse();
    fclose(yyin);
    if (result == 0 && program_root) {
        std::cout << "--- Compilation Successful ---" << std::endl;
        std::cout << "Generated TAC:" << std::endl;
        program_root->generateTAC();
        delete program_root;
        return 0;
    } else {
        std::cerr << "Compilation failed." << std::endl;
        return 1;
    }
}