#pragma once
#include<stack>
#include <string>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "nodetype.h"
#include "scope.h"
#include "matrix.hpp"
using namespace std;

class record
{
public:
    static Scope globalScope;
};

namespace ast
{
    static const char LowestPriority = '$';
    extern bool isInit;
    extern map<string,int> BinOpPriority;

    static void Init();
    static bool canpush(stack<string> &, const string &);
    static bool isNum(const char &);
    static bool isBinOp(const char &);
    static bool isBinOp(const string &);
    static bool isLetter(const char &);
    static BasicNode* __toAST(string &, Scope*);
    static void __output(BasicNode*, ostream &, const string& = string(ast::LowestPriority, 1));

    void output(BasicNode*, ostream & = cout);
    void outputASTstruct(BasicNode*, int = 0);
    BasicNode* toAST(string, Scope* = &record::globalScope);
}
