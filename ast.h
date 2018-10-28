    #pragma once
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>
#include "nodetype.h"
#include "scope.h"
using namespace std;

class record
{
public:
    static Scope globalScope;
};

namespace ast
{
    void Init();
    BasicNode* ToAST(string);
    bool canpush(stack<string> &, string);
    //冷漠：好像没啥毛病
    bool isNum(const char &);
    bool isBinOp(const char &);
    bool isBinOp(const string &);
    bool isLetter(const char &);
    extern map<string,int> BinOpPriority;
}
