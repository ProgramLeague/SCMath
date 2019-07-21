#include "ast.h"
#include "funinterface.h"

using namespace ast;

Scope record::globalScope;
bool ast::isInit = false;
map<string,int> ast::BinOpPriority;
map<int, bool> ast::BinOpCombination;

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
    addFun(BuiltinFunc::onestr,BuiltinFunc::puts,"puts",1);

    //Function* entity=runtime::globalScope.functionList["+"]; //在parse阶段，可以这样从函数域中找到函数名对应的函数实体
    //FunNode* testNode=new FunNode(entity); //然后这样通过函数实体创建相应的函数节点
    BinOpPriority[sLowestPriority] = 0;
    BinOpPriority["="] = 10;
    BinOpPriority["+"] = 20;
    BinOpPriority["-"] = 20;
    BinOpPriority["*"] = 30;
    BinOpPriority["/"] = 30;
    BinOpPriority["^"] = 40;

    //结合性false代表从右向左，true代表从左向右
    BinOpCombination[0] = true;
    BinOpCombination[10] = false;
    BinOpCombination[20] = true;
    BinOpCombination[30] = true;
    BinOpCombination[40] = true;
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

int ast::StringToType(const string& s)
{
    if (s == "num") return Num;
    if (s == "string") return String;
    return Null;
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
        if (inputstring[i] == '\"')
        {
            i++;
            while (i < inputstring.size() && inputstring[i] != '\"')
            {
                i++;
                if(i < n && inputstring[i] == '\\')
                    i++;
            }
        }
        if (i + 2 < n)
            if (inputstring.substr(i, 3) == "if(")
            {
                while (i < n && inputstring[i] != '{')
                    i++;
                //此时i为if段的开头‘{’
                int countleftParenthesis = 1;
                while (i < n && countleftParenthesis) {
                    i++;
                    if (inputstring[i] == '{')
                        countleftParenthesis++;
                    if (inputstring[i] == '}')
                        countleftParenthesis--;
                }
                if (i == n)
                    throw "Err string";
                //此时i为if的开头‘i’，j为if的结尾‘}’
                if (i < n && inputstring.substr(i + 1, 5) == "else{")
                {
                    i = i + 5;
                    countleftParenthesis = 1;
                    while (i < n && countleftParenthesis) {
                        i++;
                        if (inputstring[i] == '{')
                            countleftParenthesis++;
                        if (inputstring[i] == '}')
                            countleftParenthesis--;
                    }
                }
                if (i == n)
                    throw "Err string";
                //此时i为if的开头‘i’，j为else的结尾‘}’
            }
        if (i + 5 < n)
            if (inputstring.substr(i, 6) == "while(")
            {
                while (i < n && inputstring[i] != '{')
                    i++;
                int countleftParenthesis = 1;
                while (i < n && countleftParenthesis) {
                    i++;
                    if (inputstring[i] == '{')
                        countleftParenthesis++;
                    if (inputstring[i] == '}')
                        countleftParenthesis--;
                }
                if (i == n)
                    throw "Err string";
                //此时i为while的开头‘w’，j为while的结尾‘}’
            }
        if (i < n && inputstring[i] == '{')
        {
            int countleftParenthesis = 1;
            while (i < n && countleftParenthesis > 0)
            {
                i++;
                if (inputstring[i] == '{')
                    countleftParenthesis++;
                if (inputstring[i] == '}')
                    countleftParenthesis--;
            }
        }
        if (inputstring[i] == cLowestPriority)
        {
            input.push_back(inputstring.substr(0, i));
            inputstring = inputstring.substr(i + 1);
            i = -1;
            n = inputstring.size();
        }
        if (i == n - 1)
            input.push_back(inputstring);
    }

    for (auto& s : input)
    {
        s += sLowestPriority;
        vector<BasicNode*> token;
        vector<string> BinOp;
        string::size_type n = s.size(), i = 0;
        int highPriority = 0;
        bool ReturnFlag = false;
        bool FunReturnFlag = false;
        bool DefineFunFlag = false;

        if (s[0] == cReturnFlag)//此句为返回值
        {
            ReturnFlag = true;
            i++;
        }
        if (s[0] == cDenineFunFlag)//此句为函数定义
        {
            DefineFunFlag = true;
            i++;
        }

        for (; i < n; i++)
        {
            string temp;
            string::size_type j = i;

            //数字
            if ((s[j] == '+' || s[j] == '-') && (j == 0 || isBinOp(s[j - 1])))
            {
                j++;
            }//检查负号还是减号
            if (j < n && isNum(s[j]))
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
                temp = s.substr(i, j - i);
                i = j;
                token.push_back(new NumNode(stod(temp)));
            }

            else if (j < n && isLetter(s[j]))
            {
                while (j < n && (isLetter(s[j]) || isNum(s[j])))
                    j++;
                if (s.substr(i, j - i) == "if")
                {
                    i = j;
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
                    BasicNode* a = __toAST(temp = s.substr(i + 1, j - i - 1), sc);
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
                    BasicNode* b = __toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    BasicNode* c = nullptr;
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
                        c = __toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    }
                    IfNode* node = new IfNode(a,b,c);
                    token.push_back(node);
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
                        if (s[j] == '(')
                            countleftParenthesis++;
                        if (s[j] == ')')
                            countleftParenthesis--;
                    }
                    //此时j为')'
                    BasicNode* a = __toAST(temp = s.substr(i + 1, j - i - 1), sc);
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
                    BasicNode* b = __toAST(temp = s.substr(i + 1, j - i - 1), sc);
                    WhileNode* node   = new WhileNode(a,b);
                    token.push_back(node);
                    i = j + 1;
                }

                else if (j < n && s[j] == '(') //函数
                {
                    string name = s.substr(i, j - i);
                    if (DefineFunFlag)//函数定义
                    {
                        vector<pair<int, Variable*>> parList;
                        vector<bool> isQuote;
                        i = j;//此时i为(
                        string type, parName;
                        Scope* FunSc = new Scope(sc);
                        while (j < n && j - 1 >= 0 && s[j - 1] != ')')
                        {
                            j++;
                            if (s[j] == cType_ParSpacer || s[j] == cType_ParSpacerQuote)
                            {
                                type = s.substr(i + 1, j - i - 1);
                                i = j;
                                if (s[j] == cType_ParSpacer)
                                    isQuote.push_back(false);
                                else
                                    isQuote.push_back(true);
                            }
                            if (s[j] == ',' || s[j] == ')')
                            {
                                parName = s.substr(i + 1, j - i - 1);
                                i = j;
                                if (parName != "")
                                {
                                    FunSc->addVariable(parName, new Variable());
                                    parList.push_back(make_pair(StringToType(type), FunSc->findVariable(parName)));
                                }
                                type = parName = "";
                            }
                        }
                        //此时ij都是)
                        BasicNode* FunBody = __toAST(temp = s.substr(i + 1, s.size() - i - 2), FunSc);//把{开始到结尾都当做函数体
                        sc->addFunction(name, new Function(FunBody, parList.size()));
                        break;
                        //对于函数定义，他是不用eval就能求值（把定义的函数放在域中）
                    }
                    else
                    {
                        FunNode* node = new FunNode(sc->findFunction(name));
                        int countleftParenthesis = 1;
                        //此时s[j] == '('
                        i = j;
                        while (countleftParenthesis != 0 && j < n)
                        {
                            j++;
                            if (j < n && (s[j] == ',' || s[j] == ')') && countleftParenthesis == 1)
                            {
                                temp = s.substr(i + 1, j - i - 1);
                                if(temp != "")
                                    node->addNode(__toAST(temp , sc));
                                if (s[j] == ')') countleftParenthesis--;
                                i = j;
                            }
                            else
                            {
                                if (s[j] == '(')
                                    countleftParenthesis++;
                                if (s[j] == ')')
                                    countleftParenthesis--;
                            }
                        }
                        token.push_back(node);
                        i = j + 1;
                    }
                }

                else //变量
                {
                    string name = s.substr(i, j - i);
                    Variable* v = sc->findVariable(name);
                    if (v == nullptr) //先前没有这变量
                    {
                        sc->findVariable(name, v = new Variable());
                    }
                    token.push_back(v);
                    i = j;
                }
            }

            else if (j < n && s[j] == '(')//含括号的表达式
            {
                i = j;
                int countleftParenthesis = 1;
                while (j < n && countleftParenthesis > 0)
                {
                    j++;
                    if (s[j] == '(')
                        countleftParenthesis++;
                    if (s[j] == ')')
                        countleftParenthesis--;
                }
                if (s[j] == ')')
                {
                    token.push_back(__toAST(temp = s.substr(i + 1, j - i - 1), sc));
                    i = j + 1;//+1是要过掉‘)’
                }

                else//读到了字符串尾还是没有右括号
                {
                    token.push_back(__toAST(temp = s.substr(i + 1, j - i - 2), sc));
                    i = j - 1;//应该忽略掉字符串结尾的;，便于下面判断是否能进stackOp
                }
            }

            else if (j < n && s[j] == '{')//含括号的表达式
            {
                i = j;
                int countleftParenthesis = 1;
                while (j < n && countleftParenthesis > 0)
                {
                    j++;
                    if (s[j] == '{')
                        countleftParenthesis++;
                    if (s[j] == '}')
                        countleftParenthesis--;
                }
                if (s[j] == '}')
                {
                    token.push_back(__toAST(temp = s.substr(i + 1, j - i - 1), sc));
                    i = j + 1;//+1是要过掉‘}’
                }

                else//读到了字符串尾还是没有右括号
                {
                    token.push_back(__toAST(temp = s.substr(i + 1, j - i - 2), sc));
                    i = j - 1;//应该忽略掉字符串结尾的;，便于下面判断是否能进stackOp
                }
            }

            else if (j < n && s[j] == '\"')
            {
                i = j;
                j++;
                while (j < n&& s[j] != '\"')
                {
                    j++;
                    if (j < s.size() && s[j] == '\\')
                        j++;
                }
                token.push_back(new StringNode(s.substr(i + 1, j - i - 1)));
                i = j + 1;
            }

            if (i < n && (isBinOp(s[i]) || s[i] == cLowestPriority))
            {
                BinOp.push_back(string(1, s[i]));
                highPriority = highPriority > BinOpPriority[*(BinOp.end() - 1)] ? highPriority : BinOpPriority[*(BinOp.end() - 1)];
            }
            else
                throw "Err string";
        }

        if (BinOp.size() != token.size())
            throw "Err string";
        //正常的语句是，二元运算符比操作数少一个
        //但是这里把';'放进去了

        while (highPriority)
        {
            int t = highPriority;
            highPriority = 0;
            if(BinOpCombination[t])
                for (vector<string>::size_type i = 0; i < BinOp.size(); i++)
                {
                    int now = BinOpPriority[BinOp[i]];
                    if (now == t)
                    {
                        FunNode* Op = new FunNode(sc->findFunction(BinOp[i]));
                        Op->addNode(token[i]);
                        Op->addNode(token[i + 1]);
                        token[i] = Op;
                        token.erase(token.begin() + (i + 1));
                        BinOp.erase(BinOp.begin() + i);
                        i--;
                    }
                    else
                        highPriority = highPriority > now ? highPriority : now;
                }
            else
                for (int i = (int)BinOp.size() - 1; i >= 0; i--)//因为size()返回值为无符号的
                {
                    int now = BinOpPriority[BinOp[i]];
                    if (now == t)
                    {
                        FunNode* Op = new FunNode(sc->findFunction(BinOp[i]));
                        Op->addNode(token[i]);
                        Op->addNode(token[i + 1]);
                        token[i + 1] = Op;
                        token.erase(token.begin() + i);
                        BinOp.erase(BinOp.begin() + i);
                        i++;
                    }
                    else
                        highPriority = highPriority > now ? highPriority : now;
                }
        }

        if (!token.empty())
        {
            BasicNode* r = token[0];
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
        if (s[i] == '\"')//把'\\'和他后面的看成一个字符
        {
            i++;
            while (i < s.size() && s[i] != '\"')
            {
                i++;
                if (i < s.size() && s[i] == '\\')
                    i++;
            }
        }
        if (i == s.size())
            throw "Err string";
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
