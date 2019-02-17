#pragma once
#include "nodetype.h"
#include<cmath>

class BuiltinFunc
{
private:
    static double getNum(BasicNode* node);
public:
    //判断函数参数个数是否合法
    static bool hasOneSonNode(vector<BasicNode*>&sonNode);
    static bool hasTwoSonNodes(vector<BasicNode*>&sonNode);
    //内置函数
    static BasicNode* add(vector<BasicNode*>&sonNode);
    static BasicNode* sub(vector<BasicNode*>&sonNode);
    static BasicNode* mul(vector<BasicNode*>&sonNode);
    static BasicNode* div(vector<BasicNode*>&sonNode);
    static BasicNode* pow(vector<BasicNode*>&sonNode);

    static BasicNode* sin(vector<BasicNode*>&sonNode);
    static BasicNode* cos(vector<BasicNode*>&sonNode);

    static BasicNode* log(vector<BasicNode*>&sonNode);
};
