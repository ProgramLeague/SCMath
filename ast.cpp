#include "ast.h"
#include "funinterface.h"

using namespace ast;

Scope record::globalScope;
map<string,int> ast::BinOpPriority;

void ast::Init()
{

    //初始化所有二元运算符函数实体
    Function* add=new Function(2,BinOp::isBinOp,BinOp::add);
    Function* sub=new Function(2,BinOp::isBinOp,BinOp::sub);
    Function* mul=new Function(2,BinOp::isBinOp,BinOp::mul);
    Function* div=new Function(2,BinOp::isBinOp,BinOp::div);
    //将这些函数置入函数域
    record::globalScope.addFunction("+",add);
    record::globalScope.addFunction("-",sub);
    record::globalScope.addFunction("*",mul);
    record::globalScope.addFunction("/",div);
    //Function* entity=runtime::globalScope.functionList["+"]; //在parse阶段，可以这样从函数域中找到函数名对应的函数实体
    //FunNode* testNode=new FunNode(entity); //然后这样通过函数实体创建相应的函数节点
    map<string,int> BinOpPriority;
    BinOpPriority["$"] =0;
    BinOpPriority["+"] =10;
    BinOpPriority["-"] =10;
    BinOpPriority["*"] =20;
    BinOpPriority["/"] =20;
    BinOpPriority["^"] =30;
}

bool ast::isNum(const char &c) {
    return c >= '0' && c <= '9';
}

bool ast::isBinOp(const char &c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool ast::isBinOp(const string &c) {
    return c == "+" || c == "-" || c == "*" || c == "/" || c == "^";
}

bool ast::canpush(stack<string> &stackOp, string op) {
    if (stackOp.empty()) return true;
    return BinOpPriority[op] > BinOpPriority[stackOp.top()];
}

BasicNode* ast::ToAST(string s) {
    s += '$';
    stack<BasicNode*> stackAST;
    stack<string> stackOp;
    int n = s.size();
    for (int i = 0; i < n; i++) {
        string temp;
        int j = i;
        if ((s[j] == '+' || s[j] == '-') && (j == 0 || isBinOp(s[j - 1]))) {
            temp += s[j];
        }//检查负号还是减号
        while (j < n && isNum(s[j]))
            j++;
        if (s[j] == '.' && j + 1 < n && s[j + 1] >= '0' && s[j + 1] <= '9') {
            j++;
            while (j < n && s[j] >= '0' && s[j] <= '9') {
                j++;
            }
        }
        temp += s.substr(i,j-i);
        i = j;

        stackAST.push(new NumNode(stod(temp)));
        if (canpush(stackOp, s.substr(i,1))) {
            stackOp.push(s.substr(i,1));
        }
        else {
            while (!canpush(stackOp, s.substr(i,1))) {
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
    return stackAST.top();
}
