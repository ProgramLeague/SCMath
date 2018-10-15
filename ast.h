#pragma once

#include<map>
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>

using namespace std;

namespace ast {
    class BasicNode;//基础节点
    class NumNode;//数字节点
    class ValueNode;//变量节点
    class BinOpNode;//二元运算符节点

    static map<char, int> BinOpPriority;//运算符优先级 优先级高的值大

    void Init();
    BasicNode* ToAST(string);
    bool canpush(stack<char> &, char);
}

class ast::BasicNode{
public:
    virtual ~BasicNode() {};
};

class ast::NumNode : public ast::BasicNode {
private:
    double num;
public:
    NumNode(double _num)
        : num(_num) {};
};

class ast::ValueNode : public ast::BasicNode {
private:
    string name;
    double num;
public:
    ValueNode(string & _name, double _num)
        : name(_name), num(_num) {};
};

class ast::BinOpNode :public ast::BasicNode {
private:
    char op;
    BasicNode *lhs, *rhs;
public:
    BinOpNode(char _op, BasicNode *_lhs, BasicNode *_rhs)
        : op(_op), lhs(_lhs), rhs(_rhs) {};
};
