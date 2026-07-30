#include "ast.hpp"
#include <cstdio>

extern int yyparse();
extern FILE *yyin;
extern Node *root;

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

    int result = yyparse();
    fclose(yyin);

    if (result == 0 && root) {
        interpret(root);
        return 0;
    } else {
        fprintf(stderr, "Compilation failed.\n");
        return 1;
    }
}