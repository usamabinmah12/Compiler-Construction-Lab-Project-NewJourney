%option noyywrap
%option yylineno

%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}

%%

"let"                   { return LET; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"print"                 { return PRINT; }

[0-9]+                  { yylval.num = atoi(yytext); return NUMBER; }
\"[^"\n]*\"             {
                            yylval.str = strdup(yytext + 1);
                            yylval.str[strlen(yylval.str) - 1] = '\0';
                            return STRING;
                        }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }

"=="                    { return EQ; }
"!="                    { return NE; }
"<="                    { return LE; }
">="                    { return GE; }
"<"                     { return LT; }
">"                     { return GT; }
"&&"                    { return AND; }
"||"                    { return OR; }
"!"                     { return NOT; }

"="                     { return '='; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"*"                     { return '*'; }
"/"                     { return '/'; }
"("                     { return '('; }
")"                     { return ')'; }
"{"                     { return '{'; }
"}"                     { return '}'; }
";"                     { return ';'; }

"//"[^\n]*              { /* line comment, skip */ }
[ \t\r]+                { /* skip whitespace */ }
\n                      { /* skip newline, yylineno tracks it */ }

.                       { fprintf(stderr, "Lexical error line %d: unexpected character '%s'\n", yylineno, yytext); }

%%
