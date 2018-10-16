#pragma once
#include "nodetype.h"
#include "scode.h"

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
