#include "funinterface.h"

bool BinOp::isBinOp(vector<BasicNode*>&sonNode)
{
    return (sonNode.at(0)->getType()==Num&&sonNode.at(1)->getType()==Num);
}

double BinOp::getop(BasicNode *node)
{
    return dynamic_cast<NumNode*>(node)->getNum();
}

BasicNode* BinOp::add(vector<BasicNode *> &sonNode)
{
    return new NumNode(BinOp::getop(sonNode[0])+BinOp::getop(sonNode[1]));
}

BasicNode* BinOp::sub(vector<BasicNode *> &sonNode)
{
    return new NumNode(BinOp::getop(sonNode[0])-BinOp::getop(sonNode[1]));
}

BasicNode* BinOp::mul(vector<BasicNode *> &sonNode)
{
    return new NumNode(BinOp::getop(sonNode[0])*BinOp::getop(sonNode[1]));
}

BasicNode* BinOp::div(vector<BasicNode *> &sonNode)
{
    return new NumNode(BinOp::getop(sonNode[0])/BinOp::getop(sonNode[1]));
}
