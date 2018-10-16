#pragma once
#include <map>
#include "nodetype.h"

class Scope
{
public:
    map<string,ValueNode> valueList;
    map<string,Function> functionList;
    vector<Scope*> sonScope;
};
