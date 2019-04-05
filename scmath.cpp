#include "scmath.h"

MathFunc::MathFunc(BasicNode *func)
{
    funScope = new Scope(&record::globalScope);
    stringstream ss;
    ast::output(func, ss);
    string funString;
    ss >> funString;//因为域不同所以要这样做
    funPro = ast::toAST(funString, funScope);
}

MathFunc::MathFunc(const string &func)
{
    funScope = new Scope(&record::globalScope);
    funPro = ast::toAST(func, funScope);
}

MathFunc::MathFunc(const MathFunc &func)
{
    funScope = new Scope(&record::globalScope);
    stringstream ss;
    ss << func;
    string funString;
    ss >> funString;
    funPro = ast::toAST(funString, funScope);
}

MathFunc::~MathFunc()
{
    delete funPro;
    delete funScope;
}

void MathFunc::setVal(const string &value, BasicNode *num)
{
    if(funScope->findVariable(value, true) == nullptr)
        throw string("No Val");
    funScope->findVariable(value, true)->setVal(copyHelp::copyNode(num));
}

void MathFunc::setVal(const string &value, const double &num)
{
    if(funScope->findVariable(value, true) == nullptr)
        throw string("No Val");
    funScope->findVariable(value, true)->setVal(new NumNode(num));
}

const MathFunc MathFunc::eval()
{
   return MathFunc(funPro->eval());
}

const MathFunc MathFunc::diff(const string &value)
{
    BasicNode* result = Derivation(funPro, value);
    MathFunc retn(result);
    delete result;
    return retn;
}

ostream& operator<<(ostream &os, const MathFunc &func)
{
    ast::output(func.funPro, os);
    return os;
}

void __Simplificate(BasicNode *&now)
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

    //化简各个子树并求值，注意：没有检查除数为0
    int countNodeSonNum = 0, n = tempNow->getEntity()->getParnum();
    for(int i = 0; i < n; i++)
    {
        __Simplificate(now->sonNode[i]);
        if(now->sonNode[i]->getType() == Num)
            countNodeSonNum++;
    }
    if(countNodeSonNum == n){
        BasicNode* newnow = now->eval();
        delete now;
        now = newnow;
        return;
    }

    //处理某个值为0/1的问题
    //为了防止将(0+0)+1化简为0+1就停止,这里用从叶向上的方法
    if(tempNow->getEntity()->NAME == "+")//加法某个树为0
    {
        for(int i = 0; i <= 1; i++){
            if(tempNow->sonNode[i]->getType() == Num && ((NumNode*)(tempNow->sonNode[i]))->getNum() == 0){
                BasicNode* newnow = copyHelp::copyNode(tempNow->sonNode[i ^ 1]);
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
                BasicNode* newnow = copyHelp::copyNode(tempNow->sonNode[i ^ 1]);
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
                BasicNode* newnow = new NumNode(0);
                delete now;
                now = newnow;
                return;
            }
        }
    }

    if(tempNow->getEntity()->NAME == "-")//减法右树为0
    {
        if(tempNow->sonNode[1]->getType() == Num && ((NumNode*)(tempNow->sonNode[1]))->getNum() == 0){
            BasicNode* newnow = copyHelp::copyNode(tempNow->sonNode[0]);
            delete now;
            now = newnow;
            return;
        }
    }

    if(tempNow->getEntity()->NAME == "-")//减法左树为0
    {
        if(tempNow->sonNode[1]->getType() == Num && ((NumNode*)(tempNow->sonNode[0]))->getNum() == 0){
            BasicNode* newnow = new FunNode(record::globalScope.functionList["*"]);
            BasicNode* newnowson = copyHelp::copyNode(tempNow->sonNode[1]);
            newnow->addNode(new NumNode(-1));
            newnow->addNode(newnowson);
            delete now;
            now = newnow;
            return;
        }
    }

    if(tempNow->getEntity()->NAME == "/")//除法左树为0
    {
        if(tempNow->sonNode[0]->getType() == Num && ((NumNode*)(tempNow->sonNode[0]))->getNum() == 0){
            BasicNode* newnow = new NumNode(0);
            delete now;
            now = newnow;
            return;
        }
    }
    //warn:除法右树为0应该报错
}

BasicNode* __Derivation(BasicNode* now, const string &value){
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
                retn->addNode(__Derivation(temp->sonNode[i], value));
            return retn;
        }

        if(op == "*")//乘法:f * g == f' * g + g' * f
        {
            FunNode* retn = new FunNode(record::globalScope.functionList["+"]);
            for(int i = 0; i <= 1; i++)
            {
                FunNode* retnson = new FunNode(temp->getEntity());
                retnson->addNode(__Derivation(temp->sonNode[i], value));
                retnson->addNode(copyHelp::copyNode(temp->sonNode[i ^ 1]));
                //当i==0(左树)的时候,i^1==1(右树) 当i==1(右树)的时候,i^1==0(左树)
                retn->addNode(retnson);
            }
            return retn;
        }

        if(op == "/")//除法:f / g == (f' * g - g' * f) / g ^ 2
        {
            BasicNode *f = copyHelp::copyNode(temp->sonNode[0]), *g = copyHelp::copyNode(temp->sonNode[1]);
            BasicNode *df = __Derivation(f, value), *dg = __Derivation(g, value);
            BasicNode* num2 = new NumNode(2);
            BasicNode* retn = D(D(D(df) * D(g)) - D(D(dg) * D(f))) / D(D(copyHelp::copyNode(g)) ^ D(num2));//^的优先级比较低，要加括号
            //copyHelp::copyNode(g)是防止两个g指向相同部分
            //此时f等的所有权转移到了retn里面
            return retn;
        }

        if(op == "^")//幂:(f^g)' == (f ^ g) * (g' * ln f + g * f' / f)
        {
            BasicNode *f = copyHelp::copyNode(temp->sonNode[0]), *g = copyHelp::copyNode(temp->sonNode[1]);
            BasicNode *df = __Derivation(f, value), *dg = __Derivation(g, value);
            BasicNode* lnf = new FunNode(record::globalScope.functionList["ln"]);
            lnf->addNode(copyHelp::copyNode(f));
            BasicNode* retn = D(D(f) ^ D(g)) * D(D(D(dg) * D(lnf)) + D(D(D(copyHelp::copyNode(g)) * D(df)) / D(copyHelp::copyNode(f))));
            return retn;
        }

        if(op == "sin")//正弦函数:(sin(f))' == f' * cos(f)
        {
            BasicNode *f = copyHelp::copyNode(temp->sonNode[0]);
            BasicNode *df = __Derivation(f, value);
            BasicNode* cosf = new FunNode(record::globalScope.functionList["cos"]);
            cosf->addNode(copyHelp::copyNode(f));
            BasicNode* retn = D(df) * D(cosf);
            return retn;
        }

        if(op == "cos")//余弦函数:(cos(f))' == -1 * f' * sin(f)
        {
            BasicNode *f = copyHelp::copyNode(temp->sonNode[0]);
            BasicNode *df = __Derivation(f, value);
            BasicNode *numNeg1 = new NumNode(-1);
            BasicNode* sinf = new FunNode(record::globalScope.functionList["sin"]);
            sinf->addNode(copyHelp::copyNode(f));
            BasicNode* retn = D(numNeg1) * D(D(df) * D(sinf));
            return retn;
        }

        if(op == "log")//对数函数:(log(f, g))' = g' / (g * ln(f)) - f' * ln(g) / (f * ln(f) ^ 2)
        {
            BasicNode *f = copyHelp::copyNode(temp->sonNode[0]), *g = copyHelp::copyNode(temp->sonNode[1]);
            BasicNode *df = __Derivation(f, value), *dg = __Derivation(g, value);
            BasicNode* lnf = new FunNode(record::globalScope.functionList["ln"]);
            lnf->addNode(copyHelp::copyNode(f));
            BasicNode* lng = new FunNode(record::globalScope.functionList["ln"]);
            lng->addNode(copyHelp::copyNode(g));
            BasicNode* num2 = new NumNode(2);
            BasicNode* retn = D(D(dg) / D(D(g) * D(lnf))) - D(D(df) * D(D(lng) / D(D(f) * D(D(copyHelp::copyNode(lnf)) ^ D(num2)))));
            return retn;
        }

        if(op == "ln")//以e为底对数函数(ln(f))' == f' / f
        {
            BasicNode* f = copyHelp::copyNode(temp->sonNode[0]);
            BasicNode* df = __Derivation(f, value);
            BasicNode* retn = D(df) / D(f);
            return retn;
        }
    }

    //warn:其它函数还没有写
}

BasicNode* Derivation(BasicNode *now, const string &value){
    BasicNode *retn = __Derivation(now, value);
    __Simplificate(retn);
    return retn;
}
