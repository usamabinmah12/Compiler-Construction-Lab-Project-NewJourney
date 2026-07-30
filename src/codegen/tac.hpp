#ifndef TAC_HPP
#define TAC_HPP

#include <cstdio>
#include <string>
#include "ast.hpp"

/*
 * Three Address Code (TAC) generator.
 *
 * Produces simple instructions of the form:
 *      t1 = 5 + 3
 *      x  = t1
 *      if t2 goto L1
 *      goto L2
 *      L1:
 *      ...
 *
 * Temporaries are named t1, t2, ...  Labels are L1, L2, ...
 */

class TACGenerator {
public:
    TACGenerator() : tempCount(0), labelCount(0) {}

    void generate(Node *program) {
        printf("===== Three Address Code =====\n");
        genList(program);
        printf("==============================\n");
    }

private:
    int tempCount;
    int labelCount;

    std::string newTemp()  { return "t" + std::to_string(++tempCount); }
    std::string newLabel() { return "L" + std::to_string(++labelCount); }

    static const char *opStr(char op) {
        switch (op) {
            case '+': return "+";
            case '-': return "-";
            case '*': return "*";
            case '/': return "/";
            case 'E': return "==";
            case 'N': return "!=";
            case 'L': return "<";
            case 'G': return ">";
            case 'l': return "<=";
            case 'g': return ">=";
            default:  return "?";
        }
    }

    /* generate code for an expression, return the name holding its value */
    std::string genExpr(Node *n) {
        switch (n->type) {
            case N_NUM:
                return std::to_string(n->value);

            case N_VAR:
                return n->name;

            case N_BINOP: {
                if (n->op == 'u') {              // unary minus
                    std::string a = genExpr(n->left);
                    std::string t = newTemp();
                    printf("\t%s = -%s\n", t.c_str(), a.c_str());
                    return t;
                }
                std::string a = genExpr(n->left);
                std::string b = genExpr(n->right);
                std::string t = newTemp();
                printf("\t%s = %s %s %s\n",
                       t.c_str(), a.c_str(), opStr(n->op), b.c_str());
                return t;
            }

            default:
                return "?";
        }
    }

    void genList(Node *head) {
        for (Node *cur = head; cur; cur = cur->next) {
            genStmt(cur);
        }
    }

    void genStmt(Node *n) {
        switch (n->type) {

            case N_DECL:
            case N_ASSIGN: {
                std::string v = genExpr(n->left);
                printf("\t%s = %s\n", n->name.c_str(), v.c_str());
                break;
            }

            case N_PRINT: {
                std::string v = genExpr(n->left);
                printf("\tprint %s\n", v.c_str());
                break;
            }

            case N_PRINT_STR:
                printf("\tprint \"%s\"\n", n->name.c_str());
                break;

            case N_IF: {
                std::string cond = genExpr(n->left);
                if (n->third) {
                    /* if-else */
                    std::string Lelse = newLabel();
                    std::string Lend  = newLabel();
                    printf("\tifFalse %s goto %s\n", cond.c_str(), Lelse.c_str());
                    genList(n->right);                 // then
                    printf("\tgoto %s\n", Lend.c_str());
                    printf("%s:\n", Lelse.c_str());
                    genList(n->third);                 // else
                    printf("%s:\n", Lend.c_str());
                } else {
                    /* if only */
                    std::string Lend = newLabel();
                    printf("\tifFalse %s goto %s\n", cond.c_str(), Lend.c_str());
                    genList(n->right);
                    printf("%s:\n", Lend.c_str());
                }
                break;
            }

            case N_WHILE: {
                std::string Lstart = newLabel();
                std::string Lend   = newLabel();
                printf("%s:\n", Lstart.c_str());
                std::string cond = genExpr(n->left);
                printf("\tifFalse %s goto %s\n", cond.c_str(), Lend.c_str());
                genList(n->right);                     // body
                printf("\tgoto %s\n", Lstart.c_str());
                printf("%s:\n", Lend.c_str());
                break;
            }

            default:
                break;
        }
    }
};

#endif // TAC_HPP
