#ifndef AST_HPP
#define AST_HPP

#include <cstdio>
#include <cstdlib>
#include <string>

/* ---------- AST node ---------- */

enum NodeType {
    N_NUM,
    N_VAR,
    N_BINOP,
    N_ASSIGN,
    N_PRINT,
    N_IF,
    N_WHILE
};

struct Node {
    NodeType type;
    int value = 0;              // used by N_NUM
    std::string name;           // used by N_VAR, N_ASSIGN
    char op = 0;                // used by N_BINOP: + - * / u E N L G l g
    Node *left = nullptr;       // expr operand / cond / assign-expr / print-expr
    Node *right = nullptr;      // expr operand / then-branch head / while-body head
    Node *third = nullptr;      // else-branch head (N_IF only)
    Node *next = nullptr;       // next statement in a statement list
};

/* ---------- AST constructors ---------- */

inline Node *make_num(int val) {
    Node *n = new Node();
    n->type = N_NUM;
    n->value = val;
    return n;
}

inline Node *make_var(const std::string &name) {
    Node *n = new Node();
    n->type = N_VAR;
    n->name = name;
    return n;
}

inline Node *make_binop(char op, Node *l, Node *r) {
    Node *n = new Node();
    n->type = N_BINOP;
    n->op = op;
    n->left = l;
    n->right = r;
    return n;
}

inline Node *make_assign(const std::string &name, Node *expr) {
    Node *n = new Node();
    n->type = N_ASSIGN;
    n->name = name;
    n->left = expr;
    return n;
}

inline Node *make_print(Node *expr) {
    Node *n = new Node();
    n->type = N_PRINT;
    n->left = expr;
    return n;
}

inline Node *make_if(Node *cond, Node *thenb, Node *elseb) {
    Node *n = new Node();
    n->type = N_IF;
    n->left = cond;
    n->right = thenb;
    n->third = elseb;
    return n;
}

inline Node *make_while(Node *cond, Node *body) {
    Node *n = new Node();
    n->type = N_WHILE;
    n->left = cond;
    n->right = body;
    return n;
}

/* statement lists are just Node objects threaded via ->next */
inline Node *append_stmt(Node *list, Node *stmt) {
    if (!list) return stmt;
    Node *cur = list;
    while (cur->next) cur = cur->next;
    cur->next = stmt;
    return list;
}

/* ---------- symbol table + evaluator ---------- */

namespace ast_detail {

#define MAX_VARS 100

struct Var {
    std::string name;
    int value;
};

inline Var vars[MAX_VARS];
inline int var_count = 0;

inline int find_var(const std::string &name) {
    for (int i = 0; i < var_count; i++) {
        if (vars[i].name == name) return i;
    }
    return -1;
}

inline int get_var(const std::string &name) {
    int idx = find_var(name);
    if (idx == -1) {
        fprintf(stderr, "Runtime error: undefined variable '%s'\n", name.c_str());
        exit(1);
    }
    return vars[idx].value;
}

inline void set_var(const std::string &name, int value) {
    int idx = find_var(name);
    if (idx != -1) {
        vars[idx].value = value;
        return;
    }
    if (var_count >= MAX_VARS) {
        fprintf(stderr, "Runtime error: too many variables\n");
        exit(1);
    }
    vars[var_count].name = name;
    vars[var_count].value = value;
    var_count++;
}

inline int eval_expr(Node *n) {
    switch (n->type) {
        case N_NUM:
            return n->value;
        case N_VAR:
            return get_var(n->name);
        case N_BINOP: {
            if (n->op == 'u') { // unary minus
                return -eval_expr(n->left);
            }
            int l = eval_expr(n->left);
            int r = eval_expr(n->right);
            switch (n->op) {
                case '+': return l + r;
                case '-': return l - r;
                case '*': return l * r;
                case '/':
                    if (r == 0) {
                        fprintf(stderr, "Runtime error: division by zero\n");
                        exit(1);
                    }
                    return l / r;
                case 'E': return l == r;
                case 'N': return l != r;
                case 'L': return l < r;
                case 'G': return l > r;
                case 'l': return l <= r;
                case 'g': return l >= r;
                default:
                    fprintf(stderr, "Runtime error: unknown operator '%c'\n", n->op);
                    exit(1);
            }
        }
        default:
            fprintf(stderr, "Runtime error: node is not an expression\n");
            exit(1);
    }
    return 0; // unreachable
}

inline void exec_list(Node *head);

inline void exec_stmt(Node *n) {
    switch (n->type) {
        case N_ASSIGN:
            set_var(n->name, eval_expr(n->left));
            break;
        case N_PRINT:
            printf("%d\n", eval_expr(n->left));
            break;
        case N_IF:
            if (eval_expr(n->left)) exec_list(n->right);
            else exec_list(n->third);
            break;
        case N_WHILE:
            while (eval_expr(n->left)) exec_list(n->right);
            break;
        default:
            fprintf(stderr, "Runtime error: node is not a statement\n");
            exit(1);
    }
}

inline void exec_list(Node *head) {
    for (Node *cur = head; cur; cur = cur->next) {
        exec_stmt(cur);
    }
}

} // namespace ast_detail

inline void interpret(Node *program) {
    ast_detail::exec_list(program);
}

#endif // AST_HPP