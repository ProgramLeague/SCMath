#include "funinterface.h"
#include<cmath>

bool BuiltinFunc::hasOneSonNode(vector<BasicNode*> &sonNode)
{
    return sonNode.at(0)->getType() == Num;
}

bool BuiltinFunc::hasTwoSonNodes(vector<BasicNode*> &sonNode)
{
    return sonNode.at(0)->getType() == Num && sonNode.at(1)->getType() == Num;
}

double BuiltinFunc::getNum(BasicNode *node)
{
    return dynamic_cast<NumNode*>(node)->getNum();
}

BasicNode* BuiltinFunc::add(vector<BasicNode *> &sonNode)
{
    return new NumNode(BuiltinFunc::getNum(sonNode[0]) + BuiltinFunc::getNum(sonNode[1]));
}

BasicNode* BuiltinFunc::sub(vector<BasicNode *> &sonNode)
{
    return new NumNode(BuiltinFunc::getNum(sonNode[0]) - BuiltinFunc::getNum(sonNode[1]));
}

BasicNode* BuiltinFunc::mul(vector<BasicNode *> &sonNode)
{
    return new NumNode(BuiltinFunc::getNum(sonNode[0]) * BuiltinFunc::getNum(sonNode[1]));
}

BasicNode* BuiltinFunc::div(vector<BasicNode *> &sonNode)
{
    return new NumNode(BuiltinFunc::getNum(sonNode[0]) / BuiltinFunc::getNum(sonNode[1]));
}

BasicNode* BuiltinFunc::pow(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::pow(BuiltinFunc::getNum(sonNode[0]), BuiltinFunc::getNum(sonNode[1])));
}

BasicNode* BuiltinFunc::sin(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::sin(BuiltinFunc::getNum(sonNode[0])));
}

BasicNode* BuiltinFunc::log(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::log(BuiltinFunc::getNum(sonNode[0])) / std::log(BuiltinFunc::getNum(sonNode[1])));
}
