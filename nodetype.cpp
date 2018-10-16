#include "nodetype.h"

BasicNode::~BasicNode()
{
    for(BasicNode* node:this->sonNode)
    {
        if(node->getType()!=Variable) //这个随着域释放，不被连环析构
            delete node;
    }
}

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
    if(!this->funEntity->isVLP()) //支持变长参数就先不进行参数个数检查
        if(this->sonNode.size()+1>this->funEntity->getParnum())
            throw string("Exceeding the number of function parameters");

    this->sonNode.push_back(node);
}

void iterEval(BasicNode* &node)
{
    if(node->getType()==Pro) //按正常函数里面不要套Pro，不过考虑到某些原因……这里做个检查，以后奇葩的支持了也方便改
        throw string("ProNode cannot be function's sonNode");
    else
    {
        BasicNode* result=node->eval();
        delete node;
        node=result; //节点的替换在这里（父节点）完成，子节点只需要返回即可
    }
}

BasicNode* Function::eval(vector<BasicNode *> &sonNode)
{
    //对所有参数求值
    for(BasicNode* &node:sonNode)
    {
        iterEval(node);
    }

    //函数求值（fix:这个是错的）
    if(this->iscanBE) //基础求值模式
    {
        if(this->canBEfun(sonNode)) //参数合法
            return this->BEfun(sonNode);
        else
            throw string("sonNode type mismatch");
    }
    else //不能基础求值就是正常有函数体Pro的
    {
        vector<BasicNode*>&funbody=this->pronode->sonNode;
        for(int i=0;i<funbody.size()-1;i++) //最后一个可能是返回值，先留着后面单独处理
        {
            iterEval(funbody.at(i));
            if(funbody.at(i)->getRet())
                return funbody.at(i);
        }
        //前面都不是返回值，最后一个是
        BasicNode* lastnode=funbody.at(funbody.size()-1);
        if(lastnode==nullptr)
            return nullptr;
        else
        {
            iterEval(lastnode);
            return lastnode;
        }
    }
}

BasicNode* FunNode::eval()
{
    return this->funEntity->eval(this->sonNode);
}
