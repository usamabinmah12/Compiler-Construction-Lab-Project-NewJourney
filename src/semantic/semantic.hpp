#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <cstdio>
#include "ast.hpp"
#include "symtab.hpp"

class SemanticAnalyzer {
public:
    SemanticAnalyzer() : errors(0) {}

    int analyze(Node *program) {
        errors = 0;
        checkList(program);
        if (errors == 0)
            printf("Semantic analysis: OK (no errors)\n");
        else
            printf("Semantic analysis: %d error(s) found\n", errors);
        return errors;
    }

    SymbolTable &table() { return symtab; }

private:
    SymbolTable symtab;
    int errors;

    void error(int line, const char *msg, const std::string &name) {
        fprintf(stderr, "Semantic error (line %d): %s '%s'\n",
                line, msg, name.c_str());
        errors++;
    }

    ExprType checkExpr(Node *n) {
        if (!n) return T_UNKNOWN;

        switch (n->type) {
            case N_NUM:
                n->etype = T_INT;
                return T_INT;

            case N_VAR: {
                Symbol *s = symtab.lookup(n->name);
                if (!s) {
                    error(n->line, "use of undeclared variable", n->name);
                    n->etype = T_INT;
                    return T_INT;
                }
                n->etype = s->type;
                return s->type;
            }

            case N_BINOP: {
                if (n->op == 'u') {          // unary minus
                    ExprType t = checkExpr(n->left);
                    if (t == T_BOOL) {
                        error(n->line, "cannot apply '-' to a boolean value", "");
                    }
                    n->etype = T_INT;
                    return T_INT;
                }
                if (n->op == 'n') {          // unary logical NOT
                    ExprType t = checkExpr(n->left);
                    if (t == T_INT) {
                        error(n->line, "cannot apply '!' to a non-boolean value", "");
                    }
                    n->etype = T_BOOL;
                    return T_BOOL;
                }

                ExprType lt = checkExpr(n->left);
                ExprType rt = checkExpr(n->right);

                if (isArithmetic(n->op)) {
                    /* + - * /  need int operands, produce int */
                    if (lt == T_BOOL || rt == T_BOOL) {
                        error(n->line,
                              "arithmetic operator used on a boolean value", "");
                    }
                    n->etype = T_INT;
                    return T_INT;
                } else if (isLogical(n->op)) {
                    /* && ||  need bool operands, produce bool */
                    if (lt == T_INT || rt == T_INT) {
                        error(n->line,
                              "logical operator used on a non-boolean value", "");
                    }
                    n->etype = T_BOOL;
                    return T_BOOL;
                } else {
                    /* == != < > <= >=  compare ints, produce bool */
                    if (lt == T_BOOL || rt == T_BOOL) {
                        error(n->line,
                              "relational operator used on a boolean value", "");
                    }
                    n->etype = T_BOOL;
                    return T_BOOL;
                }
            }

            default:
                error(n->line, "invalid expression", "");
                return T_UNKNOWN;
        }
    }

    static bool isArithmetic(char op) {
        return op == '+' || op == '-' || op == '*' || op == '/';
    }

    static bool isLogical(char op) {
        return op == '&' || op == '|';
    }

    void checkList(Node *head) {
        for (Node *cur = head; cur; cur = cur->next) {
            checkStmt(cur);
        }
    }

    void checkStmt(Node *n) {
        switch (n->type) {

            case N_DECL: {
                ExprType t = checkExpr(n->left);
                if (!symtab.declare(n->name, t, n->line)) {
                    error(n->line, "redeclaration of variable", n->name);
                }
                break;
            }

            case N_ASSIGN: {
                Symbol *s = symtab.lookup(n->name);
                if (!s) {
                    error(n->line, "assignment to undeclared variable", n->name);
                    checkExpr(n->left);
                    break;
                }
                ExprType t = checkExpr(n->left);
                if (s->type != T_UNKNOWN && t != T_UNKNOWN && s->type != t) {
                    error(n->line, "type mismatch in assignment to", n->name);
                }
                break;
            }

            case N_PRINT:
                checkExpr(n->left);
                break;

            case N_PRINT_STR:
                break;

            case N_IF: {
                ExprType c = checkExpr(n->left);
                if (c != T_BOOL) {
                    error(n->line, "if condition must be a boolean expression", "");
                }
                symtab.enterScope();
                checkList(n->right);
                symtab.exitScope();
                if (n->third) {
                    symtab.enterScope();
                    checkList(n->third);
                    symtab.exitScope();
                }
                break;
            }

            case N_WHILE: {
                ExprType c = checkExpr(n->left);
                if (c != T_BOOL) {
                    error(n->line, "while condition must be a boolean expression", "");
                }
                symtab.enterScope();
                checkList(n->right);
                symtab.exitScope();
                break;
            }

            default:
                error(n->line, "invalid statement", "");
        }
    }
};

#endif // SEMANTIC_HPP