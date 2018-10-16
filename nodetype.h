#pragma once
#include <string>
#include <vector>
#include <functional>
using namespace std;

enum nodeType{Basic,Num,String,Variable,Pro,Fun};

class BasicNode //不可直接创建对象
{
public:
    virtual int getType(){return Basic;}
    virtual void addNode(BasicNode* node) {this->sonNode.push_back(node);} //使用该方法添加成员可查错

    vector<BasicNode*> sonNode;
};
typedef function<bool(vector<BasicNode*>&sonNode)>canBE; //检测函数基础求值参数表是否合法
typedef function<BasicNode*(vector<BasicNode*>&sonNode)>BE; //进行基础求值


class NumNode : public BasicNode
{
protected:
    double num;
public:
    virtual int getType() {return Num;}
    virtual void addNode(BasicNode *node) {throw string("NumNode no sonNode");}
    NumNode(double num) {this->num=num;}

    double getNum() {return this->num;}
};


class StringNode : public BasicNode
{
protected:
    string str;
public:
    virtual int getType() {return String;}
    virtual void addNode(BasicNode *node) {throw string("String no sonNode");}
    StringNode(string str) {this->str=str;}

    string getStr() {return this->str;}
};

class VariableNode : public BasicNode
{
protected:
    BasicNode val;
    bool isempty=true;
public:
    virtual int getType() {return Variable;}
    virtual void addNode(BasicNode* node) {throw string("VariableNode no sonNode");}

    bool isEmpty() {return this->isempty;}
    void setVal(BasicNode val);
    BasicNode getVal() {return this->val;}
    void clearVal() {this->isempty=true;}
};


class ProNode : public BasicNode
{
public:
    virtual int getType() {return Pro;}

    //BasicNode* getHeadNode() {return this->sonNode.at(0);}
    BasicNode* getSen(int sub) {return this->sonNode.at(sub);}
};


class Function
{
private:
    int parnum; //参数个数
    ProNode* pronode; //函数体每句的AST按顺序排列（怎么表达返回值？）
    bool VLP; //是否不进行参数个数检查
    //关于基础求值
    canBE canBEfun;
    BE BEfun;
    bool iscanBE=false;
public:
    Function(int parnum,ProNode* pronode,bool VLP=false):parnum(parnum),pronode(pronode),VLP(VLP){}

    ProNode* getPronode() {return this->pronode;}
    int getParnum() {return this->parnum;}
    bool isVLP() {return this->VLP;}
    //关于基础求值
    void setBasicEvaluation(canBE canBEfun,BE BEfun); //该函数一般仅调用一次
    BasicNode* basicEval(vector<BasicNode *> &sonNode);
};


class FunNode : public BasicNode
{
protected:
    Function* funEntity;
public:
    virtual int getType() {return Fun;}
    virtual void addNode(BasicNode* node);
    FunNode(Function* funEntity):funEntity(funEntity){}

    ProNode* getFunBody() {return this->funEntity->getPronode();}
    BasicNode* eval(); //求值之后返回值应当替换该节点
};
