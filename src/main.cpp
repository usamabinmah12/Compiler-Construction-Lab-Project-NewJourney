#include "ast.hpp"
#include "semantic.hpp"
#include "tac.hpp"
#include <cstdio>

extern int yyparse();
extern FILE *yyin;
extern Node *root;
extern int syntaxErrorCount;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.nj>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("fopen");
        return 1;
    }

    yyparse();  // may recover from syntax errors and keep going (see "error ';'" rule)
    fclose(yyin);

    if (syntaxErrorCount > 0) {
        fprintf(stderr, "Compilation failed: %d syntax error(s).\n", syntaxErrorCount);
        return 1;
    }
    if (!root) {
        fprintf(stderr, "Compilation failed: no program parsed.\n");
        return 1;
    }

    printf("--- Parsing Successful ---\n");
    printASTList(root);

    SemanticAnalyzer sem;
    int errors = sem.analyze(root);
    sem.table().print();

    if (errors > 0) {
        fprintf(stderr, "Compilation failed: %d semantic error(s).\n", errors);
        return 1;
    }

    TACGenerator tac;
    tac.generate(root);

    printf("--- Running the program ---\n");
    interpret(root);

    return 0;
}