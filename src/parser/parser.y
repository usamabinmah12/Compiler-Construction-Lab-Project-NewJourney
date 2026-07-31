%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.hpp"
int yylex(void);
int yyparse(void);
void yyerror(const char *s);
extern int yylineno;
extern FILE *yyin;
Node *root = NULL;
int syntaxErrorCount = 0;
%}
%union {
    int num;
    char *str;
    struct Node *node;
}
%token <num> NUMBER
%token <str> ID
%token <str> STRING
%token LET IF ELSE WHILE PRINT
%token EQ NE LE GE LT GT
%token AND OR NOT
%type <node> program stmt_list stmt expr
%left OR
%left AND
%right NOT
%nonassoc EQ NE LE GE LT GT
%left '+' '-'
%left '*' '/'
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
      LET ID '=' expr ';'                              { $$ = make_decl($2, $4); $$->line = yylineno; }
    | ID '=' expr ';'                                   { $$ = make_assign($1, $3); $$->line = yylineno; }
    | PRINT '(' expr ')' ';'                            { $$ = make_print($3); $$->line = yylineno; }
    | PRINT STRING ';'                                  { $$ = make_print_str($2); $$->line = yylineno; }
    | IF '(' expr ')' '{' stmt_list '}' ELSE '{' stmt_list '}' { $$ = make_if($3, $6, $10); $$->line = yylineno; }
    | IF '(' expr ')' '{' stmt_list '}'                 { $$ = make_if($3, $6, NULL); $$->line = yylineno; }
    | WHILE '(' expr ')' '{' stmt_list '}'              { $$ = make_while($3, $6); $$->line = yylineno; }
    | error ';'                                         { yyerrok; $$ = NULL; }
    ;
expr:
      NUMBER                { $$ = make_num($1); $$->line = yylineno; }
    | ID                    { $$ = make_var($1); $$->line = yylineno; }
    | expr '+' expr         { $$ = make_binop('+', $1, $3); $$->line = yylineno; }
    | expr '-' expr         { $$ = make_binop('-', $1, $3); $$->line = yylineno; }
    | expr '*' expr         { $$ = make_binop('*', $1, $3); $$->line = yylineno; }
    | expr '/' expr         { $$ = make_binop('/', $1, $3); $$->line = yylineno; }
    | expr EQ expr          { $$ = make_binop('E', $1, $3); $$->line = yylineno; }
    | expr NE expr          { $$ = make_binop('N', $1, $3); $$->line = yylineno; }
    | expr LT expr          { $$ = make_binop('L', $1, $3); $$->line = yylineno; }
    | expr GT expr          { $$ = make_binop('G', $1, $3); $$->line = yylineno; }
    | expr LE expr          { $$ = make_binop('l', $1, $3); $$->line = yylineno; }
    | expr GE expr          { $$ = make_binop('g', $1, $3); $$->line = yylineno; }
    | expr AND expr         { $$ = make_binop('&', $1, $3); $$->line = yylineno; }
    | expr OR expr          { $$ = make_binop('|', $1, $3); $$->line = yylineno; }
    | NOT expr              { $$ = make_binop('n', $2, NULL); $$->line = yylineno; }
    | '-' expr %prec UMINUS { $$ = make_binop('u', $2, NULL); $$->line = yylineno; }
    | '(' expr ')'          { $$ = $2; }
    ;
%%
void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
    syntaxErrorCount++;
}
