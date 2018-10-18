#include "nodetype.h"

bool isLiteral(BasicNode* node) //添加新的字面量要进行修改
{
    return (node->getType()==Num||node->getType()==String);
}

BasicNode* copyLiteral(BasicNode* oriVal)
{
    //调用前应该调用isLiteral对参数进行检查
    if(oriVal->getType()==Num)
        return new NumNode(dynamic_cast<NumNode*>(oriVal));
    if(oriVal->getType()==String)
        return new StringNode(dynamic_cast<StringNode*>(oriVal));
    //支持更多字面量类型后还需要在此处进行添加
    return nullptr; //如果进行参数检查了不会走到这一步
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
    if(val->getType()==Pro||val->getType()==Fun)
        throw string("Type of val cannot be used as value");
    //warn:理论上讲按照目前的设计，变量不应作为具有所有权的值（因为所有权在运行时域），但在此暂不进行检查。如果进行检查，直接在此处添加
    //fix:目前暂不支持函数指针，因为函数实体的变量表示还没设计好（函数也应该只能进行无所有权的传递）
    this->valtype=val->getType();
    this->isownership=true;
    this->val=val;
}

void VarNode::setBorrowVal(BasicNode *val)
{
    if(val->getType()==Pro||val->getType()==Fun)
        throw string("Type of val cannot be used as value");
    this->valtype=val->getType();
    this->isownership=false;
    this->val=val;
}

void VarNode::setVarVal(VarNode *node)
{
    if(node->isEmpty())
        throw string("Variable do not have values");
    BasicNode* oriVal=node->eval();
    //目前策略为：字面量进行拷贝（有所有权），变量作为无所有权指针传递
    if(isLiteral(oriVal))
        this->setVal(copyLiteral(oriVal));
    if(oriVal->getType()==Var)
        this->setBorrowVal(oriVal);
    //支持函数之后还需要在此处进行添加
}

BasicNode* VarNode::eval()
{
    if(this->isEmpty())
        return dynamic_cast<BasicNode*>(this);
    else
        return this->val;
    //注意，多级指针也只会解包一次。不过从返回值基本无法判断返回的是自身还是自身的变量指针值，所以先前需要getValType进行判断
}

void VarNode::clearVal()
{
    //调用前应进行是否为空的检查
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
