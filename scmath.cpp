#include "scmath.h"

void Simplificate(BasicNode *&now)
{
    if(now->getType() != Fun)
        return;
    FunNode* tempNow = (FunNode*)now;

    //处理加减乘除的拆括号问题
    if(tempNow->getEntity()->NAME  ==  "*" || tempNow->getEntity()->NAME == "/")
    {
        if(tempNow->sonNode[0]->getType() == Fun){
            FunNode* tempson = (FunNode*)(tempNow->sonNode[0]);
            if(tempson->getEntity()->NAME == "+" || tempson->getEntity()->NAME == "-")
            {
                //处理 (1+2)*3 (1+2)/3 乘除法左操作树为加减法这种情况
                FunNode* newnow = new FunNode(tempson->getEntity());
                for(int i = 0; i <= 1; i++){
                    FunNode* newson = new FunNode(tempNow->getEntity());
                    newson->addNode(copyHelp::copyNode(tempson->sonNode[i]));
                    newson->addNode(copyHelp::copyNode(tempNow->sonNode[1]));
                    newnow->addNode(newson);
                }
                delete now;
                now = newnow;
                tempNow = (FunNode*)now;
            }
        }
    }

    if(tempNow->getEntity()->NAME  ==  "*")
    {
        if(tempNow->sonNode[1]->getType() == Fun){
            FunNode* tempson = (FunNode*)(tempNow->sonNode[1]);
            if(tempson->getEntity()->NAME == "+" || tempson->getEntity()->NAME == "-")
            {
                //处理 1*(2+3) 乘法右操作树为加减法这种情况
                FunNode* newnow = new FunNode(tempson->getEntity());
                for(int i = 0; i <= 1; i++)
                {
                    FunNode* newson = new FunNode(tempNow->getEntity());
                    newson->addNode(copyHelp::copyNode(tempNow->sonNode[0]));
                    newson->addNode(copyHelp::copyNode(tempson->sonNode[i]));
                    newnow->addNode(newson);
                }
                delete now;
                now = newnow;
                tempNow = (FunNode*)now;
            }
        }
    }

    //化简各个子树
    for(int i = 0; i < tempNow->getEntity()->getParnum(); i++)
        Simplificate(now->sonNode[i]);

    //处理某个值为0/1的问题
    //为了防止将(0+0)+1化简为0+1就停止,这里用从叶向上的方法
    if(tempNow->getEntity()->NAME == "+")//加法某个树为0
    {
        for(int i = 0; i <= 1; i++){
            if(tempNow->sonNode[i]->getType() == Num && ((NumNode*)(tempNow->sonNode[i]))->getNum() == 0){
                BasicNode * newnow = copyHelp::copyNode(tempNow->sonNode[i ^ 1]);
                delete now;
                now = newnow;
                return;
            }
        }
    }

    if(tempNow->getEntity()->NAME == "*")//乘法某个树为1
    {
        for(int i = 0; i <= 1; i++){
            if(tempNow->sonNode[i]->getType() == Num && ((NumNode*)(tempNow->sonNode[i]))->getNum() == 1){
                BasicNode * newnow = copyHelp::copyNode(tempNow->sonNode[i ^ 1]);
                delete now;
                now = newnow;
                return;
            }
        }
    }
    if(tempNow->getEntity()->NAME == "*")//乘法某个树为0
    {
        for(int i = 0; i <= 1; i++){
            if(tempNow->sonNode[i]->getType() == Num && ((NumNode*)(tempNow->sonNode[i]))->getNum() == 0){
                BasicNode * newnow = new NumNode(0);
                delete now;
                now = newnow;
                return;
            }
        }
    }

    if(tempNow->getEntity()->NAME == "-")//减法右树为0
    {
        if(tempNow->sonNode[1]->getType() == Num && ((NumNode*)(tempNow->sonNode[1]))->getNum() == 0){
            BasicNode * newnow = copyHelp::copyNode(tempNow->sonNode[0]);
            delete now;
            now = newnow;
            return;
        }
    }
    //减法左树为0不变

    if(tempNow->getEntity()->NAME == "/")//除法左树为0
    {
        if(tempNow->sonNode[0]->getType() == Num && ((NumNode*)(tempNow->sonNode[0]))->getNum() == 0){
            BasicNode * newnow = new NumNode(0);
            delete now;
            now = newnow;
            return;
        }
    }
    //warn:除法右树为0应该报错
}

BasicNode* Derivation(BasicNode* now, const string &value){
    if(now->getType() == Num)//数字的导数为0
        return new NumNode(0);
    if(now->getType() == Var && ((VarNode*)now)->NAME != value)//其它变量的导数为0
        return new NumNode(0);
    if(now->getType() == Var && ((VarNode*)now)->NAME == value)//自己的导数为1
        return new NumNode(1);

    if(now->getType() == Fun){
        FunNode* temp = (FunNode*)now;
        string op = temp->getEntity()->NAME;

        if(op == "+" || op == "-")//加减法:f ± g == f' ± g'
        {
            FunNode* retn = new FunNode(temp->getEntity());
            for(int i = 0; i <= 1; i++)
                retn->addNode(Derivation(temp->sonNode[i], value));
            return retn;
        }

        if(op == "*")//乘法:f * g == f' * g + g' * f
        {
            FunNode* retn = new FunNode(record::globalScope.functionList["+"]);
            for(int i = 0; i <= 1; i++)
            {
                FunNode* retnson = new FunNode(temp->getEntity());
                retnson->addNode(Derivation(temp->sonNode[i], value));
                retnson->addNode(copyHelp::copyNode(temp->sonNode[i ^ 1]));
                //当i==0(左树)的时候,i^1==1(右树) 当i==1(右树)的时候,i^1==0(左树)
                retn->addNode(retnson);
            }
            return retn;
        }

        if(op == "/")//除法:f / g == (f' * g - g' * f) / g ^ 2
        {
            BasicNode *f = temp->sonNode[0], *g = temp->sonNode[1];
            FunNode* retn = new FunNode(temp->getEntity());
            FunNode* retnson = new FunNode(record::globalScope.functionList["-"]);
            FunNode* retnsonson = new FunNode(record::globalScope.functionList["*"]);//忽略这个粗糙的变量名称
            retnsonson->addNode(Derivation(f, value));
            retnsonson->addNode(copyHelp::copyNode(g));
            retnson->addNode(retnsonson);//(f' * g - 部分
            retnsonson = new FunNode(record::globalScope.functionList["*"]);
            retnsonson->addNode(Derivation(g, value));
            retnsonson->addNode(copyHelp::copyNode(f));
            retnson->addNode(retnsonson);//g' * f) 部分
            retn->addNode(retnson);//返回的树的分子部分
            retnson = new FunNode(record::globalScope.functionList["^"]);
            retnson->addNode(copyHelp::copyNode(g));
            retnson->addNode(new NumNode(2));
            retn->addNode(retnson);//分母部分
            return retn;
        }

        if(op == "^")//幂:(f^g)' == (f ^ g) * (g' * ln f + g * f' / f)
        {
            BasicNode *f = temp->sonNode[0], *g = temp->sonNode[1];
            FunNode* retn = new FunNode(record::globalScope.functionList["*"]);
            FunNode* retnson = new FunNode(temp->getEntity());
            FunNode* retnsonson, *retnsonsonson;
            retnson->addNode(copyHelp::copyNode(f));
            retnson->addNode(copyHelp::copyNode(g));
            retn->addNode(retnson);//(f^g) * 部分
            retnson = new FunNode(record::globalScope.functionList["+"]);
            retnsonson = new FunNode(record::globalScope.functionList["*"]);
            retnsonson->addNode(Derivation(g, value));
            retnsonsonson = new FunNode(record::globalScope.functionList["log"]);
            retnsonsonson->addNode(new NumNode(std::exp(1)));
            retnsonsonson->addNode(copyHelp::copyNode(f));
            retnsonson->addNode(retnsonsonson);
            retnson->addNode(retnsonson);//(g' * ln f + 部分
            retnsonson = new FunNode(record::globalScope.functionList["*"]);
            retnsonsonson = new FunNode(record::globalScope.functionList["/"]);
            retnsonsonson->addNode(Derivation(f, value));
            retnsonsonson->addNode(copyHelp::copyNode(f));
            retnsonson->addNode(copyHelp::copyNode(g));
            retnsonson->addNode(retnsonsonson);
            retnson->addNode(retnsonson);//g * f' / f) 部分
            retn->addNode(retnson);
            return retn;
        }

        if(op == "sin")//正弦函数:(sin(f))' == f' * cos(f)
        {
            BasicNode* f = temp->sonNode[0];
            FunNode* retn = new FunNode(record::globalScope.functionList["*"]);
            FunNode* retnson = new FunNode(record::globalScope.functionList["cos"]);
            retnson->addNode(copyHelp::copyNode(f));//cos(f)部分
            retn->addNode(Derivation(f, value));
            retn->addNode(retnson);
            return retn;
        }

        if(op == "cos")//余弦函数:(cos(f))' == -1 * f' * sin(f)
        {
            BasicNode* f = temp->sonNode[0];
            FunNode* retn = new FunNode(record::globalScope.functionList["*"]);
            FunNode* retnson = new FunNode(record::globalScope.functionList["*"]);
            FunNode* retnsonson = new FunNode(record::globalScope.functionList["sin"]);
            retnsonson->addNode(copyHelp::copyNode(f));//sin(f)部分
            retnson->addNode(Derivation(f, value));
            retnson->addNode(retnsonson);//f' * sin(f)部分
            retn->addNode(new NumNode(-1));
            retn->addNode(retnson);
            return retn;
        }

        if(op == "log")//对数函数:(log(f, g))' = g' / (g * log(e, f) - f' * log(e, g) / (f * log(e, f) ^ 2)
        {
            //咕咕咕
        }
    }

    //warn:其它函数还没有写
}
