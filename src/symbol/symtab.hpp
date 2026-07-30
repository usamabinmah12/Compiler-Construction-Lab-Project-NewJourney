#ifndef SYMTAB_HPP
#define SYMTAB_HPP

#include <string>
#include <vector>
#include <cstdio>
#include "ast.hpp"

/*
 * Symbol Table with nested scopes.
 *
 * Each scope is one level of { } (or the global level). A scope is a list
 * of symbols. We keep a stack of scopes: entering a block pushes a new
 * scope, leaving a block pops it. Lookup walks from the innermost scope
 * outward, so inner variables can shadow outer ones.
 */

struct Symbol {
    std::string name;
    ExprType    type;   // T_INT or T_BOOL
    int         line;   // line where it was declared
};

class SymbolTable {
public:
    SymbolTable() {
        enterScope();   // start with the global scope
    }

    /* open a new inner scope (called when we enter a { } block) */
    void enterScope() {
        scopes.push_back(std::vector<Symbol>());
    }

    /* close the innermost scope (called when we leave a { } block) */
    void exitScope() {
        if (!scopes.empty()) scopes.pop_back();
    }

    /*
     * Declare a variable in the CURRENT (innermost) scope.
     * Returns false if it already exists in this same scope
     * (that is a redeclaration).
     */
    bool declare(const std::string &name, ExprType type, int line) {
        std::vector<Symbol> &cur = scopes.back();
        for (size_t i = 0; i < cur.size(); i++) {
            if (cur[i].name == name) {
                return false;   // already declared in this scope
            }
        }
        Symbol s;
        s.name = name;
        s.type = type;
        s.line = line;
        cur.push_back(s);
        return true;
    }

    /*
     * Look a name up, searching from the innermost scope outward.
     * Returns a pointer to the symbol, or nullptr if not declared anywhere.
     */
    Symbol *lookup(const std::string &name) {
        for (int i = (int)scopes.size() - 1; i >= 0; i--) {
            std::vector<Symbol> &sc = scopes[i];
            for (size_t j = 0; j < sc.size(); j++) {
                if (sc[j].name == name) return &sc[j];
            }
        }
        return nullptr;
    }

    /* pretty-print the whole table (for debugging / the report) */
    void print() {
        printf("===== Symbol Table =====\n");
        for (size_t i = 0; i < scopes.size(); i++) {
            printf("Scope %zu:\n", i);
            for (size_t j = 0; j < scopes[i].size(); j++) {
                Symbol &s = scopes[i][j];
                printf("  %-12s  type=%s  line=%d\n",
                       s.name.c_str(),
                       s.type == T_BOOL ? "bool" : "int",
                       s.line);
            }
        }
        printf("========================\n");
    }

private:
    std::vector<std::vector<Symbol>> scopes;
};

#endif // SYMTAB_HPP
