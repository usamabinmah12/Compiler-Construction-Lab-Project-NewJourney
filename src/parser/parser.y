%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
int yyparse(void);
void yyerror(const char *s);
extern int yylineno;
extern FILE *yyin;

Node *root = NULL;
%}

%union {
    int num;
    char *str;
    struct Node *node;
}

%token <num> NUMBER
%token <str> ID
%token LET IF ELSE WHILE PRINT
%token EQ NE LE GE LT GT

%type <node> program stmt_list stmt expr

%left '+' '-'
%left '*' '/'
%nonassoc EQ NE LE GE LT GT
%right UMINUS

%%

program:
    stmt_list { root = $1; }
    ;

stmt_list:
    /* empty */          { $$ = NULL; }
    | stmt_list stmt      { $$ = append_stmt($1, $2); }
    ;

stmt:
      LET ID '=' expr ';'                              { $$ = make_assign($2, $4); }
    | ID '=' expr ';'                                   { $$ = make_assign($1, $3); }
    | PRINT '(' expr ')' ';'                            { $$ = make_print($3); }
    | IF '(' expr ')' '{' stmt_list '}' ELSE '{' stmt_list '}' { $$ = make_if($3, $6, $10); }
    | IF '(' expr ')' '{' stmt_list '}'                 { $$ = make_if($3, $6, NULL); }
    | WHILE '(' expr ')' '{' stmt_list '}'              { $$ = make_while($3, $6); }
    ;

expr:
      NUMBER                { $$ = make_num($1); }
    | ID                    { $$ = make_var($1); }
    | expr '+' expr         { $$ = make_binop('+', $1, $3); }
    | expr '-' expr         { $$ = make_binop('-', $1, $3); }
    | expr '*' expr         { $$ = make_binop('*', $1, $3); }
    | expr '/' expr         { $$ = make_binop('/', $1, $3); }
    | expr EQ expr          { $$ = make_binop('E', $1, $3); }
    | expr NE expr          { $$ = make_binop('N', $1, $3); }
    | expr LT expr          { $$ = make_binop('L', $1, $3); }
    | expr GT expr          { $$ = make_binop('G', $1, $3); }
    | expr LE expr          { $$ = make_binop('l', $1, $3); }
    | expr GE expr          { $$ = make_binop('g', $1, $3); }
    | '-' expr %prec UMINUS { $$ = make_binop('u', $2, NULL); }
    | '(' expr ')'          { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("fopen");
            return 1;
        }
    }
    if (yyparse() == 0) {
        interpret(root);
    }
    return 0;
}
