#pragma once
#include <map>
#include "nodetype.h"

class Scope
{
public:
    ~Scope();
    map<string,VariableNode*> variableList;
    map<string,Function*> functionList;
    vector<Scope*> sonScope;

    void addValue(string name) {this->variableList.insert(name,new VariableNode);} //先插进去后赋值
    void addFunction(string name,Function* fun) {this->functionList.insert(name,fun);} //这个Function必须在外面先new好，因为参数比较多
};
