%{
#include "ast/ast.hpp"
#include <stdio.h>
extern int yylex();
extern int yyparse();
void yyerror(const char *s) { fprintf(stderr, "Syntax Error: %s\n", s); }
ASTNode *program_root;
%}
%union {
    char *str_val;
    ASTNode *ast_val;
}
%token PRINT
%token <str_val> STRING
%token SEMICOLON
%type <ast_val> program statement print_stmt
%%
program: statement { program_root = $1; };
statement: print_stmt { $$ = $1; };
print_stmt: PRINT STRING SEMICOLON { $$ = new PrintStmt($2); };
%%