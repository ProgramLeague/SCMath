#pragma once
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>
//#include<map>
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

    #ifdef parserdebug
    void outputAST(BasicNode *);
    #endif

    extern map<string,int> BinOpPriority;
}
