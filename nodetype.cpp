#include "nodetype.h"

bool isLiteral(BasicNode* node) //添加新的字面量要进行修改
{
    return (node->getType()==Num||node->getType()==String);
}

BasicNode::~BasicNode()
{
    for(BasicNode* node:this->sonNode)
    {
        if(node->getType()!=Var) //这个随着域释放，不被连环析构
            delete node;
    }
}


VarNode::~VarNode()
{
    if((!this->isEmpty())&&this->isownership)
        delete this->val;
    //然后BasicNode析构
}

void VarNode::setVal(BasicNode* val)
{
    if(val->getType()==Pro)
        throw String("Pro cannot be used as value");
    //warn:理论上讲按照目前的设计，变量和函数不应作为具有所有权的值，但在此暂不进行检查。如果进行检查，直接在此处添加
    this->valtype=val->getType();
    this->isownership=true;
    this->val=val;
}

void VarNode::setBorrowVal(BasicNode *val)
{
    if(val->getType()==Pro)
        throw String("Pro cannot be used as value");
    this->valtype=val->getType();
    this->isownership=false;
    this->val=val;
}

BasicNode* VarNode::eval()
{
    if(this->isEmpty())
        return dynamic_cast<BasicNode*>(this);
    else
        return this->val;
    //注意，多级指针也只会解包一次
}

void VarNode::clearVal()
{
    //这里不进行是否为空的检查了
    this->valtype=-1;
    if(this->isownership)
        delete this->val;
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
    if(node->getType()==Pro) //按正常函数里面不要套Pro
        throw string("ProNode cannot be function's sonNode");
    else
    {
        if(isLiteral(node))
            return; //如果是字面量，自己就是求值结果，下面再重新赋值一次就重复了
        else
        {
            BasicNode* result=node->eval();
            if(node->getType()!=Var)
                delete node;
            node=result; //节点的替换在这里（父节点）完成，子节点只需要返回即可
            //对于已经赋值的变量，整体过程是用值替代本身变量在AST中的位置，不过变量本身并没有被析构，因为变量的所有权在scope（后面可能还要访问）
            //对于未赋值的变量，求值结果是变量本身，AST没有变化
        }
    }
}

BasicNode* Function::eval(vector<BasicNode *> &sonNode)
{
    //对所有参数求值
    for(BasicNode* &node:sonNode)
        iterEval(node);

    //函数求值
    if(this->iscanBE) //基础求值模式
    {
        if(this->canBEfun(sonNode)) //参数合法
            return this->BEfun(sonNode);
        else
            throw string("sonNode type mismatch");
    }
    else //不能基础求值就是正常有函数体Pro的（fix:从这里开始就是错的）
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
    if(this->funEntity==nullptr)
        throw String("funEntity is null");
    return this->funEntity->eval(this->sonNode);
}
