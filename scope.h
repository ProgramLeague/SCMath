#pragma once
#include <map>
#include "nodetype.h"

class Scope
{
public:
    ~Scope();
    map<string,VariableNode*> valueList;
    map<string,Function*> functionList;
    vector<Scope*> sonScope;
};
