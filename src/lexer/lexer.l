%{
#include "parser.tab.h"
#include <string.h>
%}
%option noyywrap
%%
"print"         { return PRINT; }
\"[^\"]*\"      { 
                    yylval.str_val = strdup(yytext + 1);
                    yylval.str_val[strlen(yylval.str_val) - 1] = '\0';
                    return STRING; 
                 }
";"             { return SEMICOLON; }
[ \t\n\r]       { /* ignore whitespace */ }
.               { fprintf(stderr, "Lexical error: unrecognized '%c'\n", yytext[0]); }
%%