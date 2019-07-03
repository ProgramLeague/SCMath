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
    static const char cLowestPriority = ';';
    static const string sLowestPriority = ";";
    extern bool isInit;
    extern map<string,int> BinOpPriority;

    void Init();
    bool canpush(stack<string> &, const string &);
    bool isNum(const char &);
    bool isBinOp(const char &);
    bool isBinOp(const string &);
    bool isLetter(const char &);
    BasicNode* __toAST(string &, Scope*);
    void __output(BasicNode*, ostream &, const string& = sLowestPriority);

    void output(BasicNode*, ostream & = cout);
    BasicNode* toAST(string, Scope* = &record::globalScope);
}
