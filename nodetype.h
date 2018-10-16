#pragma once
#include <string>
#include <vector>
#include <functional>
using namespace std;

enum nodeType{Basic,Num,String,Variable,Pro,Fun};

class BasicNode //不可直接创建对象
{
protected:
    bool isRet=false;
public:
    virtual int getType(){return Basic;}
    virtual void addNode(BasicNode* node) {this->sonNode.push_back(node);} //使用该方法添加成员可查错
    virtual BasicNode* eval();

    void setRet() {this->isRet=true;} //不可eval节点设置ret无效
    bool getRet() {return this->isRet;}
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
    virtual BasicNode* eval() {return dynamic_cast<BasicNode*>(this);}
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
    virtual BasicNode* eval() {return dynamic_cast<BasicNode*>(this);}
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
    virtual BasicNode* eval() {return dynamic_cast<BasicNode*>(this);}

    bool isEmpty() {return this->isempty;}
    void setVal(BasicNode val);
    BasicNode getVal() {return this->val;}
    void clearVal() {this->isempty=true;}
};


class ProNode : public BasicNode
{
public:
    virtual int getType() {return Pro;}
    virtual BasicNode* eval() {throw string("ProNode cannot be eval");}

    //BasicNode* getHeadNode() {return this->sonNode.at(0);}
    BasicNode* getSen(int sub) {return this->sonNode.at(sub);}
};


class Function
{
private:
    int parnum; //参数个数
    ProNode* pronode; //是ret节点返回，最后一个元素视为返回值（如果没有填nullptr）
    bool VLP; //是否不进行参数个数检查
    //关于基础求值
    canBE canBEfun;
    BE BEfun;
    bool iscanBE=false;
    //BasicNode* basicEval(vector<BasicNode *> &sonNode); //直接在eval里写了，这里消掉
public:
    Function(int parnum,ProNode* pronode=nullptr,bool VLP=false):parnum(parnum),pronode(pronode),VLP(VLP){} //普通函数（有函数体）
    Function(int parnum,canBE canBEfun,BE BEfun,bool VLP=false):
        parnum(parnum),canBEfun(canBEfun),BEfun(BEfun),VLP(VLP),iscanBE(true){} //调用到函数接口

    ProNode* getPronode() {return this->pronode;}
    int getParnum() {return this->parnum;}
    bool isVLP() {return this->VLP;}
    BasicNode* eval(vector<BasicNode *> &sonNode);
};


class FunNode : public BasicNode
{
protected:
    Function* funEntity;
public:
    virtual int getType() {return Fun;}
    virtual void addNode(BasicNode* node);
    virtual BasicNode* eval();
    FunNode(Function* funEntity):funEntity(funEntity){}

    ProNode* getFunBody() {return this->funEntity->getPronode();}
};
