#include "ast.h"
#include "funinterface.h"
#include "runtime.h"
using namespace ast;

void ast::Init()
{
    //初始化所有二元运算符函数实体
    Function* add=new Function(2,BinOp::isBinOp,BinOp::add);
    Function* sub=new Function(2,BinOp::isBinOp,BinOp::sub);
    Function* mul=new Function(2,BinOp::isBinOp,BinOp::mul);
    Function* div=new Function(2,BinOp::isBinOp,BinOp::div);
    //将这些函数置入函数域
    runtime::globalScope.addFunction("+",add);
    runtime::globalScope.addFunction("-",sub);
    runtime::globalScope.addFunction("*",mul);
    runtime::globalScope.addFunction("/",div);
    //Function* entity=runtime::globalScope.functionList["+"]; //在parse阶段，可以这样从函数域中找到函数名对应的函数实体
    //FunNode* testNode=new FunNode(entity); //然后这样通过函数实体创建相应的函数节点
}

bool isNum(const char &c) {
    return c >= '0' && c <= '9';
}

bool isBinOp(const char &c) {
    return c == '+' && c == '-' && c == '*' && c == '/' && c == '^';
}

bool ast::canpush(stack<char> &stackOp, char op) {
    if (stackOp.empty()) return true;
    return BinOpPriority[op] > BinOpPriority[stackOp.top()];
}

BasicNode* ast::ToAST(string s) {
    s += '$';
    stack<BasicNode*> stackAST;
    stack<char> stackOp;
    int n = s.size();
    for (int i = 0; i < n; i++) {
        string temp;
        int j = i;
        if ((s[j] == '+' || s[j] == '-') && (j == 0 || isBinOp(s[j - 1]))) {
            temp += s[j];
        }//检查负号还是减号
        while (j < n && isNum(s[j])) {
            temp += s[j];
            j++;
        }
        if (s[j] == '.' && j + 1 < n && s[j + 1] >= '0' && s[j + 1] <= '9') {
            temp += s[j];
            j++;
            while (j < n && s[j] >= '0' && s[j] <= '9') {
                temp += s[j];
                j++;
            }
        }
        i = j;
        /*
        while (i < n && s[i] >= '0' && s[i] <= '9') {
            x = x * 10 + s[i] - '0';
            i++;
        }
        if (s[i] == '.' && i + 1 < n && s[i + 1] >= '0' && s[i + 1] <= '9') {
            i++;
            while (i < n && s[i] >= '0' && s[i] <= '9') {
                x = x * 10 + s[i] - '0';
                x /= 10.0;
                i++;
            }
        }
        */
        stackAST.push(new NumNode(stod(temp)));
        if (canpush(stackOp, s[i])) {
            stackOp.push(s[i]);
        }
        else {
            while (!canpush(stackOp, s[i])) {
                BasicNode* a = stackAST.top();
                stackAST.pop();
                BasicNode* b = stackAST.top();
                stackAST.pop();
                char c = stackOp.top();
                stackOp.pop();
                switch (c) {
                case '+':
                    stackAST.push(new BinOpNode('+',b,a));
                    break;
                case '-':
                    stackAST.push(new BinOpNode('-', b, a));
                    break;
                case '*':
                    stackAST.push(new BinOpNode('*', b, a));
                    break;
                case '/':
                    stackAST.push(new BinOpNode('/', b, a));
                    break;
                case '^':
                    stackAST.push(new BinOpNode('^', b, a));
                }
            }
            stackOp.push(s[i]);
        }
    }
    return stackAST.top();
}
