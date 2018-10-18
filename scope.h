#pragma once
#include <map>
#include "nodetype.h"

class Scope //作用是记录不同语句块内函数及变量名到实体的映射，不是运行时栈
{
public:
    ~Scope();
    map<string,Variable*> variableList;
    map<string,Function*> functionList;
    vector<Scope*> sonScope;

    void addValue(string name) {this->variableList[name]=new Variable;} //先插进去后赋值
    void addFunction(string name,Function* fun) {this->functionList[name]=fun;} //这个Function必须在外面先new好，因为参数比较多
};
