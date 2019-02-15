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
    //将这些函数置入函数域
    record::globalScope.addFunction("+",add);
    record::globalScope.addFunction("-",sub);
    record::globalScope.addFunction("*",mul);
    record::globalScope.addFunction("/",div);
    record::globalScope.addFunction("^",pow);
    record::globalScope.addFunction("sin",sin);
    //Function* entity=runtime::globalScope.functionList["+"]; //在parse阶段，可以这样从函数域中找到函数名对应的函数实体
    //FunNode* testNode=new FunNode(entity); //然后这样通过函数实体创建相应的函数节点
    BinOpPriority["$"] =0;
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
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool ast::isLetter(const char &c)
{
    return (c >= 'A' && c <='Z') || (c >= 'a' && c <= 'z');
}

bool ast::canpush(stack<string> &stackOp, string op)
{
    if (stackOp.empty()) return true;
    return BinOpPriority[op] > BinOpPriority[stackOp.top()];
}

BasicNode* ast::__ToAST(string &s)
{
    s += '$';
    stack<BasicNode*> stackAST;
    stack<string> stackOp;
    int n = s.size();

    for (int i = 0; i < n; i++)
    {
        string temp;
        int j = i;

        if ((s[j] == '+' || s[j] == '-') && (j == 0 || isBinOp(s[j - 1])))
       {
            temp += s[j];
        }//检查负号还是减号
        if(j < n && isNum(s[j]))
        {
            while (j < n && isNum(s[j]))
                j++;//整数部分
           if (s[j] == '.' && j + 1 < n && isNum(s[j + 1])) {
                j++;//小数部分
                while (j < n && isNum(s[j + 1]))
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
            if(j < n && s[j] == '(')//函数
            {
                 FunNode* node = new FunNode(record::globalScope.functionList[s.substr(i, j - i)]);
                 //此时s[j] == '('
                 while(s[j] != ')' && s[j] != '$')
                 {
                     i = j;
                     j++;
                    while(j < n && s[j] != ',' && s[j] != ')' && s[j] != '$')
                        j++;
                    node->addNode(ToAST(s.substr(i + 1, j - i - 1)));
                 }
               stackAST.push(node);
               i = j + 1;
            }

            else//变量
            {
                record::globalScope.addVariable(s.substr(i, j -  i));
                stackAST.push(record::globalScope.variableList[s.substr(i, j - i)]);
                i = j;
            }
        }

        else if(j < n && s[j] == '(')//含括号的表达式
        {
            i = j;
            while(j < n && s[j] != ')')
                j++;
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


        if(i < n && (isBinOp(s[i]) || s[i] == '$'))//按理说，一个数字/变量/函数结束之后是一个运算符/字符串结尾
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
        if(*c == '\x20' || * c == '\n' || *c == 't'){
            s.erase(c);
            c--;
        }
    }
    return __ToAST(s);
}
