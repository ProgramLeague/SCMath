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

void Function::setBasicEvaluation(canBE canBEfun, BE BEfun)
{
    this->iscanBE=true;
    this->canBEfun=canBEfun;
    this->BEfun=BEfun;
}

BasicNode* Function::basicEval(vector<BasicNode *>&sonNode)
{
    if(!this->iscanBE)
        throw string("BE undefined");
    if(this->canBEfun(sonNode))
    {
        return this->BEfun(sonNode);
    }
    else
        throw string("sonNode type mismatch");
}

void FunNode::addNode(BasicNode *node)
{
    if(!this->funEntity->isVLP()) //支持变长参数就先不进行参数个数检查
        if(this->sonNode.size()+1>this->funEntity->getParnum())
            throw string("Exceeding the number of function parameters");

    this->sonNode.push_back(node);
}

BasicNode* FunNode::eval()
{
    //fix:这里递归，还需要考虑
}
