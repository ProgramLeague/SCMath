#include "ast.h"
#include "funinterface.h"

using namespace ast;

Scope record::globalScope;
bool ast::isInit = false;
map<string,int> ast::BinOpPriority;

void addFun(canBE canBEfun,BE BEfun,string name,int parnum=-1)
{
    record::globalScope.addFunction(name, new Function(canBEfun, BEfun,parnum));
}

void ast::Init()
{
    //初始化所有内置函数实体
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::add,"+",2);
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::sub,"-",2);
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::mul,"*",2);
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::div,"/",2);
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::pow,"^",2);
    addFun(BuiltinFunc::hasOneSonNode, BuiltinFunc::sin,"sin",1);
    addFun(BuiltinFunc::hasOneSonNode, BuiltinFunc::cos,"cos",1);
    addFun(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::log,"log",2);
    addFun(BuiltinFunc::hasOneSonNode, BuiltinFunc::ln,"ln",1);

    addFun(BuiltinFunc::assignmentCheck, BuiltinFunc::assignment,"=",2);
    //fix:目前=是通过局部求值实现的，即在被赋值变量原先没有值的情况下，局部求值模式下eval结果仍然为
    //变量本身，这样传进=函数之后左边儿子就是变量本身，可以正确赋值。一旦变量已经被赋值，那进入=前
    //就会被eval为它的值，值不能赋值给值，这就错了。因此现在这个写法不好。正确的做法应该是在parse阶段
    //创建每个变量的指针（二级varnode），然后将这个指针作为=的左边儿子

    addFun(BuiltinFunc::twoVec, BuiltinFunc::vecDot,"vecDot",2);
    addFun(BuiltinFunc::twoMat, BuiltinFunc::matDot,"matDot",2);
    addFun(BuiltinFunc::twoMat, BuiltinFunc::matAdd,"matAdd",2);
    addFun(BuiltinFunc::twoVec, BuiltinFunc::vecAdd,"vecAdd",2);
    addFun(BuiltinFunc::twoVec, BuiltinFunc::vecSub,"vecSub",2);
    addFun(BuiltinFunc::twoMat, BuiltinFunc::matSub,"matSub",2);
    addFun(BuiltinFunc::vecNum, BuiltinFunc::vecMul,"vecMul",2);
    addFun(BuiltinFunc::matNum, BuiltinFunc::matMul,"matMul",2);
    addFun(BuiltinFunc::matNum, BuiltinFunc::getRVector,"getRVec",2);
    addFun(BuiltinFunc::pmatVecNum, BuiltinFunc::setRVector,"setRVec", 3);
    addFun(BuiltinFunc::oneMat, BuiltinFunc::det,"det",1);
    addFun(BuiltinFunc::oneMat, BuiltinFunc::inv,"inv",1);
    addFun(BuiltinFunc::oneMat, BuiltinFunc::adjoint,"adjoint",1);
    addFun(BuiltinFunc::matVec, BuiltinFunc::linerSolve,"linerSolve",2);
    addFun(BuiltinFunc::matNum3,BuiltinFunc::rsub,"rsub",4);
    addFun(BuiltinFunc::matNum2,BuiltinFunc::rmul,"rmul",3);
    addFun(BuiltinFunc::matNum2,BuiltinFunc::rswap,"rswap",3);

    //Function* entity=runtime::globalScope.functionList["+"]; //在parse阶段，可以这样从函数域中找到函数名对应的函数实体
    //FunNode* testNode=new FunNode(entity); //然后这样通过函数实体创建相应的函数节点
    BinOpPriority["$"] =0;
    BinOpPriority["="] =1;
    BinOpPriority["+"] =10;
    BinOpPriority["-"] =10;
    BinOpPriority["*"] =20;
    BinOpPriority["/"] =20;
    BinOpPriority["^"] =30;
}

bool ast::isNum(const char &c)
{
    return c >= '0' && c <= '9';
}

bool ast::isBinOp(const char &c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c=='=';
}

bool ast::isBinOp(const string &c)
{
    return c == "+" || c == "-" || c == "*" || c == "/" || c == "^" || c=="=";
}

bool ast::isLetter(const char &c)
{
    return (c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z');
}

bool ast::canpush(stack<string> &stackOp, const string& op)
{
    if (stackOp.empty()) return true;
    return BinOpPriority[op] > BinOpPriority[stackOp.top()];
}

BasicNode* ast::__toAST(string &inputstring, Scope* sc)
{
    vector<string> input;
    vector<BasicNode*> output;
    string::size_type n = inputstring.size();
    for (string::size_type i = 0; i < n; i++)
    {
        //按照分号把输入拆成几句话
        //注意：if和while体中的分号应该忽略掉
        string::size_type j = i;
        if(i + 2 < n)
            if (inputstring.substr(i, 3) == "if(")
            {
                while (j < n && inputstring[j] != '{')
                    j++;
                //此时j为if段的开头‘{’
                int countleftParenthesis = 1;
                while (j < n && countleftParenthesis) {
                    j++;
                    if (inputstring[j] == '{')
                        countleftParenthesis++;
                    if (inputstring[j] == '}')
                        countleftParenthesis--;
                }
                if (j == n)
                    throw "Error string";
                //此时i为if的开头‘i’，j为if的结尾‘}’
                if (j < n && inputstring.substr(j + 1, 5) == "else{")
                {
                    j = j + 5;
                    countleftParenthesis = 1;
                    while (j < n && countleftParenthesis) {
                        j++;
                        if (inputstring[j] == '{')
                            countleftParenthesis++;
                        if (inputstring[j] == '}')
                            countleftParenthesis--;
                    }
                }
                if (j == n)
                    throw "Error string";
                //此时i为if的开头‘i’，j为else的结尾‘}’
                i = j;
            }
        if(i + 5 < n)
            if (inputstring.substr(i, 6) == "while(")
            {
                while (j < n && inputstring[j] != '{')
                    j++;
                int countleftParenthesis = 1;
                while (j < n && countleftParenthesis) {
                    j++;
                    if (inputstring[j] == '{')
                        countleftParenthesis++;
                    if (inputstring[j] == '}')
                        countleftParenthesis--;
                }
                if (j == n)
                    throw "Error string";
                //此时i为while的开头‘w’，j为while的结尾‘}’
                i = j;
            }
        if (inputstring[i] == cLowestPriority)
        {
            input.push_back(inputstring.substr(0, i));
            inputstring = inputstring.substr(i + 1);
            i = 0;
            n = inputstring.size();
        }
        if (i == n - 1)
            input.push_back(inputstring);
    }

    for(auto& s : input){
        s += cLowestPriority;
        stack<BasicNode*> stackAST;
        stack<string> stackOp;
        string::size_type n = s.size(), i = 0;
        bool ReturnFlag = false;

        if (s[0] == cReturnFlag)
        {
            ReturnFlag = true;
            i++;
        }

        for (; i < n; i++)
        {
            string temp;
            string::size_type j = i;

            if ((s[j] == '+' || s[j] == '-') && (j == 0 || isBinOp(s[j - 1])))
            {
                j++;
            }//检查负号还是减号
            if(j < n && isNum(s[j]))
            {
                while (j < n && isNum(s[j]))
                    j++;//整数部分
                if (s[j] == '.' && j + 1 < n && isNum(s[j + 1])) {
                    j++;//小数部分
                    while (j < n && isNum(s[j]))
                    {
                        j++;
                    }
                }
                temp += s.substr(i,j-i);
                i = j;
                stackAST.push(new NumNode(stod(temp)));
            }

            else if (j < n && isLetter(s[j]))
            {
                while(j < n && isLetter(s[j]))
                    j++;

                if (s.substr(i, j - i) == "if")
                {
                    i = j;
                    BasicNode* condition, *truePro, *falsePro = nullptr;
                    //此时i为'('
                    int countleftParenthesis = 1;
                    while (j < s.size() && s[j] != ')')
                    {
                        j++;
                        if (s[j] == '(')
                            countleftParenthesis++;
                        if (s[j] == ')')
                            countleftParenthesis--;
                    }
                    //此时j为')'
                    condition =  toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    i = j + 1;//此时i为'{'
                    j = i;
                    countleftParenthesis = 1;
                    while (j < n && countleftParenthesis) {
                        j++;
                        if (s[j] == '{')
                            countleftParenthesis++;
                        if (s[j] == '}')
                            countleftParenthesis--;
                    }//此时j为if体结束'}'
                    truePro = toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    if (s.substr(j + 1, 4) == "else")
                    {
                        i = j + 5;//此时i为'{'
                        j = i;
                        countleftParenthesis = 1;
                        while (j < n && countleftParenthesis) {
                            j++;
                            if (s[j] == '{')
                                countleftParenthesis++;
                            if (s[j] == '}')
                                countleftParenthesis--;
                        }//此时j为else体结束'}'
                        falsePro = toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    }
                    stackAST.push(new IfNode(condition,truePro,falsePro));
                    i = j + 1;
                }

                else if (s.substr(i, j - i) == "while")
                {
                    i = j;
                    //此时i为'('
                    int countleftParenthesis = 1;
                    while (j < s.size() && s[j] != ')')
                    {
                        j++;
                        if (s[j] == '{')
                            countleftParenthesis++;
                        if (s[j] == '}')
                            countleftParenthesis--;
                    }
                    //此时j为')'
                    BasicNode* condition =  toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    i = j + 1;//此时i为'{'
                    j = i;
                    countleftParenthesis = 1;
                    while (j < n && countleftParenthesis) {
                        j++;
                        if (s[j] == '{')
                            countleftParenthesis++;
                        if (s[j] == '}')
                            countleftParenthesis--;
                    }//此时j为while体结束'}'
                    BasicNode* body = toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    stackAST.push(new WhileNode(condition, body));
                }


                else if(j < n && s[j] == '(') //函数
                {
                    string name=s.substr(i, j - i);
                    FunNode* node = new FunNode(sc->findFunction(name));
                    int countleftParenthesis = 1;
                    //此时s[j] == '('
                    i=j;
                    while(countleftParenthesis != 0 && j < n)
                    {
                        j++;
                        if(j < n && (s[j] == ','|| s[j] == ')') && countleftParenthesis == 1)
                        {
                            node->addNode(__toAST(temp = s.substr(i + 1, j - i - 1), sc));
                            if(s[j]==')') countleftParenthesis--;
                            i=j;
                        }
                        else
                        {
                            if(s[j] == '(')
                                countleftParenthesis++;
                            if(s[j] == ')')
                                countleftParenthesis--;
                        }
                    }
                    stackAST.push(node);
                    i = j + 1;
                }

                else //变量
                {
                    string name=s.substr(i, j -  i);
                    Variable* v=sc->findVariable(name);
                    if(v==nullptr) //先前没有这变量
                    {
                        v=sc->addVariable(name);
                        Variable* pv=sc->addVariable("ptr"+name);
                        pv->setBorrowVal(v);
                    }
                    stackAST.push(v);
                    i = j;
                }
            }
            else if(j < n && s[j] == '(')//含括号的表达式
            {
                i = j;
                int countleftParenthesis = 1;
                while(j < n && countleftParenthesis > 0)
                {
                    j++;
                    if(s[j] == '(')
                        countleftParenthesis++;
                    if(s[j] == ')')
                        countleftParenthesis--;
                }
                if(s[j] == ')')
                {
                    stackAST.push(__toAST(temp = s.substr(i + 1, j - i - 1), sc));//为了让参数1变为左值
                    i = j + 1;//+1是要过掉‘)’
                }
                else//读到了字符串尾还是没有右括号
                {
                    stackAST.push(__toAST(temp = s.substr(i + 1, j - i - 2), sc));
                    i = j - 1;//应该忽略掉字符串结尾的$，便于下面判断是否能进stackOp
                }
            }


            if(i < n && (isBinOp(s[i]) || s[i] == cLowestPriority))//按理说，一个数字/变量/函数结束之后是一个运算符/字符串结尾
            {
                if (canpush(stackOp, s.substr(i,1)))
                {
                    stackOp.push(s.substr(i,1));
                }
                else {
                    while (!canpush(stackOp, s.substr(i,1)))
                    {
                        BasicNode* a = stackAST.top();
                        stackAST.pop();
                        BasicNode* b = stackAST.top();
                        stackAST.pop();
                        string c = stackOp.top();
                        stackOp.pop();
                        FunNode* node = new FunNode(sc->findFunction(c));
                        node->addNode(b);
                        node->addNode(a);
                        stackAST.push(node);
                    }
                    stackOp.push(s.substr(i,1));
                }
            }
            else
            {
                throw Excep("Error string");
            }

        }
        if (!stackAST.empty())
        {
            BasicNode* r = stackAST.top();
            if (ReturnFlag)
                r->setRet();
            output.push_back(r);
        }
    }
    if (output.size() == 0)
        return new nullNode();
    if (output.size() == 1)
        return output[0];
    ProNode* retn = new ProNode();
    for (auto& i : output)
        retn->addNode(i);
    return retn;
}

BasicNode* ast::toAST(string s, Scope* sc){
    if(!isInit){
        isInit = true;
        Init();
    }
    //去掉空字符和回车
    //去掉回车时候应该特判一下，如果是"if()\n{}"（或者是else，while），把回车直接删掉
    //其他的回车换为';'
    for (string::size_type i = 0; i < s.size(); i++)
    {
        if (s[i] == '\x20' || s[i] == '\t')
        {
            s.erase(i, 1);
            i--;
        }
    }

    for (string::size_type i = 0; i < s.size(); i++)
    {
        if (s.substr(i,2) == "if")
        {
            i = i + 2;//i为f后面的字符
            while (i < s.size() && s[i] == '\n')
                s.erase(i, 1);
        }
        if (s.substr(i, 4) == "else")
        {
            i = i + 4;//i为e后面的字符
            while (i < s.size() && s[i] == '\n')
                s.erase(i, 1);
        }
        if (s.substr(i,5) == "while")
        {
            i = i + 5;
            while (i < s.size() && s[i] == '\n')
                s.erase(i, 1);
            while (i < s.size() && s[i] != ')')
                i++;
            i++;//i为while条件结束')'的下一个字符
            while (i < s.size() && s[i] == '\n')
                s.erase(i, 1);
        }
        if (s[i] == '\n')
            s[i] = cLowestPriority;
    }

    return __toAST(s, sc);
}



void ast::__output(BasicNode* now, ostream &os, const string& FatherOP)
{
    if(now->getType() == Num)
    {
        double nownum = ((NumNode*)now)->getNum();
        if(nownum < 0)
            os << '(' << nownum << ')';
        else
            os << nownum;
        return;
    }

    if(now->getType() == Fun)
    {
        FunNode* t = (FunNode*)now;
        string op = t->getEntity()->NAME;
        if(isBinOp(op))
        {
            if(ast::BinOpPriority[op] < ast::BinOpPriority[FatherOP])
                os << '(';
            __output(t->sonNode[0], os, op);
            os << op;
            if(op == "-" && t->sonNode[1]->getType() == Fun && ast::BinOpPriority[op] == ast::BinOpPriority[((FunNode*)(t->sonNode[1]))->getEntity()->NAME])
                os << '(';
            __output(t->sonNode[1], os, op);
            if(ast::BinOpPriority[op] < ast::BinOpPriority[FatherOP])
                os << ')';
            if(op == "-" && t->sonNode[1]->getType() == Fun && ast::BinOpPriority[op] == ast::BinOpPriority[((FunNode*)(t->sonNode[1]))->getEntity()->NAME])
                os << ')';
            return;
        }
        os << op << '(';
        int n = t->getEntity()->getParnum();
        for(int i = 0 ; i < n; i++)
        {
            __output(t->sonNode[i], os);
            if(i != n - 1)
                os << ",";
        }
        os << ')';
        return;
    }

    if(now->getType() == Var)
    {
        os << ((VarNode*)now)->NAME;
    }

    if(now->getType()==Matrix)
        ((matrixNode*)now)->output(os);

    if(now->getType()==Vector)
        ((vectorNode*)now)->output(os);

    if(now->getType()==Null)
        return;
}

void ast::output(BasicNode *now, ostream &os)
{
    __output(now, os);
}
