#include "nodetype.h"

void VariableNode::setVal(BasicNode num)
{
    if(num.getType()==Num||num.getType()==String)
    {
        this->isEmpty=false;
        this->num=num;
    }
    else
        throw string("The value of a variable must be literal");
}

void FunNode::addNode(BasicNode *node)
{
    if(this->sonNode.size()+1>this->funEntity->getParnum())
        throw string("Exceeding the number of function parameters");

    this->sonNode.push_back(node);
}
