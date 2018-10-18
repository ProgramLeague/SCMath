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
    bool canpush(stack<char> &, char);
}
