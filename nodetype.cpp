#include "nodetype.h"

void ValueNode::setVal(double num)
{
    this->isEmpty=false;
    this->num=num;
}

void FunNode::addNode(BasicNode *node)
{
    if(this->sonNode.size()+1>this->funEntity->getParnum())
        throw string("Exceeding the number of function parameters");

    this->sonNode.push_back(node);
}
