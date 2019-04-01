#include "funinterface.h"
#include <cmath>
#include "matrix.hpp"

//typeCheck
bool BuiltinFunc::hasOneSonNode(vector<BasicNode*> &sonNode)
{
    return sonNode.at(0)->getType() == Num;
}

bool BuiltinFunc::hasTwoSonNodes(vector<BasicNode*> &sonNode)
{
    return sonNode.at(0)->getType() == Num && sonNode.at(1)->getType() == Num;
}

bool BuiltinFunc::assignmentCheck(vector<BasicNode *> &sonNode)
{
    if(sonNode.at(0)->getType() == Var)
    {
        BasicNode* result=sonNode.at(1)->eval();
        if(copyHelp::isLiteral(result))
        {
            return true;
        }
    }
    else
        return false;
}

bool BuiltinFunc::oneMat(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix;
}

bool BuiltinFunc::twoVec(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Vector && sonNode.at(1)->getType() == Vector;
}

bool BuiltinFunc::twoMat(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix && sonNode.at(1)->getType() == Matrix;
}

bool BuiltinFunc::vecNum(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Vector && sonNode.at(1)->getType() == Num;
}

bool BuiltinFunc::matNum(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix && sonNode.at(1)->getType() == Num;
}

bool BuiltinFunc::matNum2(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix && sonNode.at(1)->getType() == Num
            && sonNode.at(2)->getType() == Num;
}

bool BuiltinFunc::matNum3(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix && sonNode.at(1)->getType() == Num
            && sonNode.at(2)->getType() == Num && sonNode.at(3)->getType() == Num;
}

bool BuiltinFunc::matVec(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Matrix && sonNode.at(1)->getType() == Vector;
}

bool BuiltinFunc::pmatVecNum(vector<BasicNode *> &sonNode)
{
    return sonNode.at(0)->getType() == Var && ((VarNode*)sonNode.at(0))->getValType() == Matrix &&
            sonNode.at(1)->getType() == Vector && sonNode.at(2)->getType() == Num;
}

double BuiltinFunc::getNum(BasicNode *node)
{
    return dynamic_cast<NumNode*>(node)->getNum();
}

//FunBody
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

BasicNode* BuiltinFunc::cos(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::cos(BuiltinFunc::getNum(sonNode[0])));
}

BasicNode* BuiltinFunc::log(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::log(BuiltinFunc::getNum(sonNode[1])) / std::log(BuiltinFunc::getNum(sonNode[0])));
}

BasicNode* BuiltinFunc::ln(vector<BasicNode *> &sonNode)
{
    return new NumNode(std::log(BuiltinFunc::getNum(sonNode[0])));
}

BasicNode* BuiltinFunc::vecDot(vector<BasicNode *> &sonNode)
{
    vectorNode* m1=(vectorNode*)sonNode[0];
    vectorNode* m2=(vectorNode*)sonNode[1];
    return new NumNode(m1->dot(*m2));
}

BasicNode* BuiltinFunc::matDot(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    matrixNode* m2=(matrixNode*)sonNode[1];
    return new matrixNode(m1->dot(*m2));
}

BasicNode* BuiltinFunc::vecAdd(vector<BasicNode *> &sonNode)
{
    vectorNode* m1=(vectorNode*)sonNode[0];
    vectorNode* m2=(vectorNode*)sonNode[1];
    return new vectorNode(m1->add(*m2));
}

BasicNode* BuiltinFunc::matAdd(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    matrixNode* m2=(matrixNode*)sonNode[1];
    return new matrixNode(m1->add(*m2));
}

BasicNode* BuiltinFunc::vecSub(vector<BasicNode *> &sonNode)
{
    vectorNode* m1=(vectorNode*)sonNode[0];
    vectorNode* m2=(vectorNode*)sonNode[1];
    vectorNode rm2=m2->mul(-1);
    return new vectorNode(m1->add(rm2));
}

BasicNode* BuiltinFunc::matSub(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    matrixNode* m2=(matrixNode*)sonNode[1];
    matrixNode rm2=m2->mul(-1);
    return new matrixNode(m1->add(rm2));
}

BasicNode* BuiltinFunc::vecMul(vector<BasicNode *> &sonNode)
{
    vectorNode* v1=(vectorNode*)sonNode[0];
    NumNode* d2=(NumNode*)sonNode[1];
    return new vectorNode(v1->mul(d2->getNum()));
}

BasicNode* BuiltinFunc::matMul(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* d2=(NumNode*)sonNode[1];
    return new matrixNode(m1->mul(d2->getNum()));
}

BasicNode* BuiltinFunc::getCVector(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* d2=(NumNode*)sonNode[1];
    return new vectorNode(m1->getCVector(d2->getNum()));
}

BasicNode* BuiltinFunc::getRVector(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* d2=(NumNode*)sonNode[1];
    return new vectorNode(m1->getRVector(d2->getNum()));
}

BasicNode* BuiltinFunc::setCVector(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)(((VarNode*)sonNode[0])->getVal());
    vectorNode* v2=(vectorNode*)sonNode[1];
    NumNode* n3=(NumNode*)sonNode[2];
    m1->setCVector(*v2,n3->getNum());
    return new nullNode();
}

BasicNode* BuiltinFunc::setRVector(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)(((VarNode*)sonNode[0])->getVal());
    vectorNode* v2=(vectorNode*)sonNode[1];
    NumNode* n3=(NumNode*)sonNode[2];
    m1->setRVector(*v2,n3->getNum());
    return new nullNode();
}

BasicNode* BuiltinFunc::det(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    return new NumNode(m1->det());
}

BasicNode* BuiltinFunc::inv(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    return new matrixNode(m1->inv());
}

BasicNode* BuiltinFunc::adjoint(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    return new matrixNode(m1->adjoint());
}

BasicNode* BuiltinFunc::linerSolve(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    vectorNode* m2=(vectorNode*)sonNode[1];
    return new vectorNode(matrixNode::solve(*m1,*m2));
}

BasicNode* BuiltinFunc::assignment(vector<BasicNode *> &sonNode)
{
    VarNode* v=(VarNode*)sonNode[0];
    v->setVal(copyHelp::copyVal(sonNode[1])); //在参数类型检查时已经进行eval
    return v->eval(); //支持yoda表达法
}

BasicNode* BuiltinFunc::rsub(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* n1=(NumNode*)sonNode[1];
    NumNode* n2=(NumNode*)sonNode[2];
    NumNode* n3=(NumNode*)sonNode[3];
    m1->rsub(n1->getNum(),n2->getNum(),n3->getNum());
    return new matrixNode(*m1);
}

BasicNode* BuiltinFunc::rmul(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* n1=(NumNode*)sonNode[1];
    NumNode* n2=(NumNode*)sonNode[2];
    m1->rmul(n1->getNum(),n2->getNum());
    return new matrixNode(*m1);
}

BasicNode* BuiltinFunc::rswap(vector<BasicNode *> &sonNode)
{
    matrixNode* m1=(matrixNode*)sonNode[0];
    NumNode* n1=(NumNode*)sonNode[1];
    NumNode* n2=(NumNode*)sonNode[2];
    m1->rswap(n1->getNum(),n2->getNum());
    return new matrixNode(*m1);
}
