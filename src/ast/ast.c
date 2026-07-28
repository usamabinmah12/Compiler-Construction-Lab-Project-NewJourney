#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ---------- simple symbol table ---------- */
#define MAX_VARS 100

typedef struct {
    char *name;
    int value;
} Var;

static Var vars[MAX_VARS];
static int var_count = 0;

static int find_var(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    return -1;
}

static int get_var(const char *name) {
    int idx = find_var(name);
    if (idx == -1) {
        fprintf(stderr, "Runtime error: undefined variable '%s'\n", name);
        exit(1);
    }
    return vars[idx].value;
}

static void set_var(const char *name, int value) {
    int idx = find_var(name);
    if (idx != -1) {
        vars[idx].value = value;
        return;
    }
    if (var_count >= MAX_VARS) {
        fprintf(stderr, "Runtime error: too many variables\n");
        exit(1);
    }
    vars[var_count].name = strdup(name);
    vars[var_count].value = value;
    var_count++;
}

/* ---------- AST node constructors ---------- */

static Node *new_node(NodeType type) {
    Node *n = calloc(1, sizeof(Node));
    if (!n) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    n->type = type;
    return n;
}

Node *make_num(int val) {
    Node *n = new_node(N_NUM);
    n->value = val;
    return n;
}

Node *make_var(char *name) {
    Node *n = new_node(N_VAR);
    n->name = strdup(name);
    return n;
}

Node *make_binop(char op, Node *l, Node *r) {
    Node *n = new_node(N_BINOP);
    n->op = op;
    n->left = l;
    n->right = r;
    return n;
}

Node *make_assign(char *name, Node *expr) {
    Node *n = new_node(N_ASSIGN);
    n->name = strdup(name);
    n->left = expr;
    return n;
}

Node *make_print(Node *expr) {
    Node *n = new_node(N_PRINT);
    n->left = expr;
    return n;
}

Node *make_if(Node *cond, Node *thenb, Node *elseb) {
    Node *n = new_node(N_IF);
    n->left = cond;
    n->right = thenb;
    n->third = elseb;
    return n;
}

Node *make_while(Node *cond, Node *body) {
    Node *n = new_node(N_WHILE);
    n->left = cond;
    n->right = body;
    return n;
}

/* statement lists are just N_* nodes threaded via ->next */
Node *append_stmt(Node *list, Node *stmt) {
    if (!list) return stmt;
    Node *cur = list;
    while (cur->next) cur = cur->next;
    cur->next = stmt;
    return list;
}

/* ---------- evaluator ---------- */

static int eval_expr(Node *n) {
    switch (n->type) {
        case N_NUM:
            return n->value;
        case N_VAR:
            return get_var(n->name);
        case N_BINOP: {
            if (n->op == 'u') { /* unary minus */
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
    return 0; /* unreachable */
}

static void exec_list(Node *head);

static void exec_stmt(Node *n) {
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

static void exec_list(Node *head) {
    for (Node *cur = head; cur; cur = cur->next) {
        exec_stmt(cur);
    }
}

void interpret(Node *program) {
    exec_list(program);
}
