#pragma once
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>
using namespace std;
#include "nodetype.h"
#include "scope.h"
#include "runtime.h"

namespace ast
{
    void Init();
    BasicNode* ToAST(string);
    bool canpush(stack<char> &, char);
}
