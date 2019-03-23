#pragma once
#include "nodetype.h"
#include <cmath>

#define TypeCheck(name) static bool name(vector<BasicNode*>&sonNode)
#define _TypeCheck(name) bool BuiltinFunc::name(vector<BasicNode*>&sonNode)
#define FunBody(name) static BasicNode* name(vector<BasicNode*>&sonNode)
#define _FunBody(name) BasicNode* BuiltinFunc::name(vector<BasicNode*>&sonNode)

class BuiltinFunc
{
private:
    static double getNum(BasicNode* node);
public:
    //判断函数参数个数是否合法
    TypeCheck(hasOneSonNode);
    TypeCheck(hasTwoSonNodes);
    TypeCheck(assignmentCheck);

    TypeCheck(oneMat);
    TypeCheck(twoVec);
    TypeCheck(twoMat);
    TypeCheck(vecNum);
    TypeCheck(matNum);
    TypeCheck(matVec);
    TypeCheck(pmatVecNum);
    //内置函数
    FunBody(add);
    FunBody(sub);
    FunBody(mul);
    FunBody(div);
    FunBody(pow);

    FunBody(sin);
    FunBody(cos);

    FunBody(log);
    FunBody(ln);

    FunBody(assignment);

    FunBody(vecDot);
    FunBody(matDot);
    FunBody(vecAdd);
    FunBody(matAdd);
    FunBody(vecSub);
    FunBody(matSub);
    FunBody(vecMul);
    FunBody(matMul);
    FunBody(getRVector);
    FunBody(getCVector);
    FunBody(setRVector);
    FunBody(setCVector);
    FunBody(det);
    FunBody(inv);
    FunBody(adjoint);
    FunBody(linerSolve);
};
