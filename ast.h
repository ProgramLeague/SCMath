#pragma once
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>
using namespace std;
#include "nodetype.h"
#include "scope.h"

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
    //3A:这是为了把output移出来才放到头文件的，不知道对不对
    bool isNum(const char &c);
    bool isBinOp(const char &c);
    bool isBinOp(const string &c);

    extern map<string,int> BinOpPriority;
}
