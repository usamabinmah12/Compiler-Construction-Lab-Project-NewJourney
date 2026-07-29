#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <string>

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual void generateTAC() = 0;
};

class StringNode : public ASTNode {
public:
    std::string value;
    StringNode(const std::string &val) : value(val) {}
    void generateTAC() override {}
};

class PrintStmt : public ASTNode {
public:
    StringNode *strExpr;
    PrintStmt(char *strVal) { strExpr = new StringNode(strVal); }
    ~PrintStmt() { delete strExpr; }
    void generateTAC() override {
        std::cout << "PRINT " << strExpr->value << std::endl;
    }
};

#endif