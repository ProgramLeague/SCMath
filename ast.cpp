#include "ast.h"
#include "funinterface.h"

using namespace ast;

Scope record::globalScope;
bool ast::isInit = false;
map<string,int> ast::BinOpPriority;

static void ast::Init()
{
    //初始化所有内置函数实体
    Function* add = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::add,2);
    Function* sub = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::sub,2);
    Function* mul = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::mul,2);
    Function* div = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::div,2);
    Function* pow = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::pow,2);
    Function* sin = new Function(BuiltinFunc::hasOneSonNode, BuiltinFunc::sin,1);
    Function* cos = new Function(BuiltinFunc::hasOneSonNode, BuiltinFunc::cos,1);
    Function* log = new Function(BuiltinFunc::hasTwoSonNodes, BuiltinFunc::log, 2);
    Function* ln = new Function(BuiltinFunc::hasOneSonNode, BuiltinFunc::ln, 1);

    Function* assignment = new Function(BuiltinFunc::assignmentCheck, BuiltinFunc::assignment, 2);

    Function* vecDot = new Function(BuiltinFunc::twoVec, BuiltinFunc::vecDot, 2);
    Function* matDot = new Function(BuiltinFunc::twoMat, BuiltinFunc::matDot, 2);
    Function* matAdd = new Function(BuiltinFunc::twoMat, BuiltinFunc::matAdd, 2);
    Function* vecAdd = new Function(BuiltinFunc::twoVec, BuiltinFunc::vecAdd, 2);
    Function* vecSub = new Function(BuiltinFunc::twoVec, BuiltinFunc::vecSub, 2);
    Function* matSub = new Function(BuiltinFunc::twoMat, BuiltinFunc::matSub, 2);
    Function* vecMul = new Function(BuiltinFunc::vecNum, BuiltinFunc::vecMul, 2);
    Function* matMul = new Function(BuiltinFunc::matNum, BuiltinFunc::matMul, 2);
    Function* getRVector = new Function(BuiltinFunc::matNum, BuiltinFunc::getRVector, 2);
    Function* getCVector = new Function(BuiltinFunc::matNum, BuiltinFunc::getCVector, 2);
    Function* setCVector = new Function(BuiltinFunc::pmatVecNum, BuiltinFunc::setCVector, 3);
    Function* setRVector = new Function(BuiltinFunc::pmatVecNum, BuiltinFunc::setRVector, 3);
    Function* det = new Function(BuiltinFunc::oneMat, BuiltinFunc::det, 1);
    Function* inv = new Function(BuiltinFunc::oneMat, BuiltinFunc::inv, 1);
    Function* adjoint = new Function(BuiltinFunc::oneMat, BuiltinFunc::adjoint, 1);
    Function* linerSolve = new Function(BuiltinFunc::twoMat, BuiltinFunc::linerSolve, 2);
    //将这些函数置入函数域
    record::globalScope.addFunction("+",add);
    record::globalScope.addFunction("-",sub);
    record::globalScope.addFunction("*",mul);
    record::globalScope.addFunction("/",div);
    record::globalScope.addFunction("^",pow);
    record::globalScope.addFunction("=",assignment);
    //fix:目前=是通过局部求值实现的，即在被赋值变量原先没有值的情况下，局部求值模式下eval结果仍然为
    //变量本身，这样传进=函数之后左边儿子就是变量本身，可以正确赋值。一旦变量已经被赋值，那进入=前
    //就会被eval为它的值，值不能赋值给值，这就错了。因此现在这个写法不好。正确的做法应该是在parse阶段
    //创建每个变量的指针（二级varnode），然后将这个指针作为=的左边儿子
    record::globalScope.addFunction("sin",sin);
    record::globalScope.addFunction("cos",cos);
    record::globalScope.addFunction("log", log);
    record::globalScope.addFunction("ln", ln);

    record::globalScope.addFunction("linerSolve", linerSolve);
    record::globalScope.addFunction("det", det);
    record::globalScope.addFunction("inv", inv);
    record::globalScope.addFunction("adjoint", adjoint);
    record::globalScope.addFunction("setRVector", setRVector);
    record::globalScope.addFunction("setCVector", setCVector);
    record::globalScope.addFunction("getCVector", getCVector);
    record::globalScope.addFunction("getRVector", getRVector);
    record::globalScope.addFunction("matMul", matMul);
    record::globalScope.addFunction("vecMul", vecMul);
    record::globalScope.addFunction("matSub", matSub);
    record::globalScope.addFunction("vecSub", vecSub);
    record::globalScope.addFunction("vecAdd", vecAdd);
    record::globalScope.addFunction("matAdd", matAdd);
    record::globalScope.addFunction("matDot", matDot);
    record::globalScope.addFunction("vecDot", vecDot);
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

BasicNode* ast::__ToAST(string &s)
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
                FunNode* node = new FunNode(record::globalScope.functionList[name]);
                int countleftParenthesis = 1;
                //此时s[j] == '('
                i=j;
                while(countleftParenthesis != 0 && j < n)
                {
                    j++;
                    if(j < n && (s[j] == ','|| s[j] == ')') && countleftParenthesis == 1)
                    {
                        node->addNode(ToAST(s.substr(i + 1, j - i - 1)));
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
                if(record::globalScope.haveVariable(name)==false) //先前没有这变量
                {
                    Variable* v=record::globalScope.addVariable(name);
                    Variable* pv=record::globalScope.addVariable("ptr"+name);
                    pv->setBorrowVal(v);
                }
                stackAST.push(record::globalScope.variableList[name]);
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
                stackAST.push(ToAST(s.substr(i + 1, j - i - 1)));
                i = j + 1;//+1是要过掉‘)’
            }
            else//读到了字符串尾还是没有右括号
            {
                stackAST.push(ToAST(s.substr(i + 1, j - i - 2)));
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
                    FunNode* node = new FunNode(record::globalScope.functionList[c]);
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

BasicNode* ast::ToAST(string s){
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
    return __ToAST(s);
}
