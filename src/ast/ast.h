#ifndef AST_H
#define AST_H

typedef enum {
    N_NUM,
    N_VAR,
    N_BINOP,
    N_ASSIGN,
    N_PRINT,
    N_IF,
    N_WHILE
} NodeType;

typedef struct Node {
    NodeType type;
    int value;              /* used by N_NUM */
    char *name;              /* used by N_VAR, N_ASSIGN */
    char op;                 /* used by N_BINOP: + - * / u E N L G l g */
    struct Node *left;       /* expr operand / cond / assign-expr / print-expr */
    struct Node *right;      /* expr operand / then-branch head / while-body head */
    struct Node *third;      /* else-branch head (N_IF only) */
    struct Node *next;       /* next statement in a statement list */
} Node;

Node *make_num(int val);
Node *make_var(char *name);
Node *make_binop(char op, Node *l, Node *r);
Node *make_assign(char *name, Node *expr);
Node *make_print(Node *expr);
Node *make_if(Node *cond, Node *thenb, Node *elseb);
Node *make_while(Node *cond, Node *body);
Node *append_stmt(Node *list, Node *stmt);

void interpret(Node *program);

#endif
