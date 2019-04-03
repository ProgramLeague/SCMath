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

static void ast::Init()
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

bool ast::isLetter(const char &c)
{
    return (c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z');
}

bool ast::canpush(stack<string> &stackOp, const string& op)
{
    if (stackOp.empty()) return true;
    return BinOpPriority[op] > BinOpPriority[stackOp.top()];
}

BasicNode* ast::__ToAST(string &s, Scope* sc)
{
    s += LowestPriority;
    stack<BasicNode*> stackAST;
    stack<string> stackOp;
    int n = s.size();

    for (int i = 0; i < n; i++)
    {
        string temp;
        int j = i;

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
            if(j < n && s[j] == '(') //函数
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
                        node->addNode(__ToAST(temp = s.substr(i + 1, j - i - 1), sc));
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
                stackAST.push(__ToAST(temp = s.substr(i + 1, j - i - 1), sc));//为了让参数1变为左值
                i = j + 1;//+1是要过掉‘)’
            }
            else//读到了字符串尾还是没有右括号
            {
                stackAST.push(__ToAST(temp = s.substr(i + 1, j - i - 2), sc));
                i = j - 1;//应该忽略掉字符串结尾的$，便于下面判断是否能进stackOp
            }
        }


        if(i < n && (isBinOp(s[i]) || s[i] == LowestPriority))//按理说，一个数字/变量/函数结束之后是一个运算符/字符串结尾
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
            throw string("Error string");
        }

    }
    return stackAST.top();
}

BasicNode* ast::ToAST(string s, Scope* sc){
    if(!isInit){
        Init();
        isInit = true;
    }
    for(string::iterator c = s.begin(); c != s.end(); c++){
        if(*c == '\x20' || * c == '\n' || *c == '\t'){
            s.erase(c);
            c--;
        }
    }
    return __ToAST(s, sc);
}
