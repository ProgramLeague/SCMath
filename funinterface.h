#pragma once
#include "nodetype.h"
#include "scope.h" //包含这个，支持函数接口直接修改运行时域内的量（产生副作用）

class BinOp
{
private:
    static double getop(BasicNode* node);
public:
    static bool isBinOp(vector<BasicNode*>&sonNode);
    static BasicNode* add(vector<BasicNode*>&sonNode);
    static BasicNode* sub(vector<BasicNode*>&sonNode);
    static BasicNode* mul(vector<BasicNode*>&sonNode);
    static BasicNode* div(vector<BasicNode*>&sonNode);
};
