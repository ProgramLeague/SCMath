#include "output.h"

using namespace output;

bool output::isBinOp(const string &c)
{
    return c == "+" || c == "-" || c == "*" || c == "/" || c == "^";
}


void output::outputAST(BasicNode* now, const string& FatherOP)
{
    if(now == nullptr)
        return;
    if(now->getType() == Num)
    {
        double nownum = ((NumNode*)now)->getNum();
        if(nownum < 0)
            cout << '(' << nownum << ')';
        else
            cout << nownum;
        return;
    }
    if(now->getType() == Fun)
    {
        FunNode* t = (FunNode*)now;
        string op = t->getEntity()->NAME;
        if(output::isBinOp(op))
        {
            if(ast::BinOpPriority[op] < ast::BinOpPriority[FatherOP])
                cout << '(';
            outputAST(t->sonNode[0], op);
            cout << op;
            if(op == "-" && t->sonNode[1]->getType() == Fun && ast::BinOpPriority[op] == ast::BinOpPriority[((FunNode*)(t->sonNode[1]))->getEntity()->NAME])
                cout << '(';
            outputAST(t->sonNode[1], op);
            if(ast::BinOpPriority[op] < ast::BinOpPriority[FatherOP])
                cout << ')';
            if(op == "-" && t->sonNode[1]->getType() == Fun && ast::BinOpPriority[op] == ast::BinOpPriority[((FunNode*)(t->sonNode[1]))->getEntity()->NAME])
                cout << ')';
            return;
        }
        cout << op << '(';
        int n = t->getEntity()->getParnum();
        for(int i = 0 ; i < n; i++)
        {
            outputAST(t->sonNode[i], string(ast::LowestPriority, 1));
            if(i != n - 1)
                cout << ",";
        }
        cout << ')';
        return;
    }
    if(now->getType() == Var)
    {
        cout << ((VarNode*)now)->NAME;
    }
}

void output::outputASTstruct(BasicNode* now, int depth)//方便输出AST结构(并没有找到别的好方法)
{
    for(int i = 0; i < depth; i++)
        cout << ' ';
    if(now->getType() == Num)
    {
        cout << ((NumNode*)now)->getNum() << endl;
        return;
    }
    if(now->getType() == Var)
    {
        cout << ((VarNode*)now)->NAME << endl;
        return;
    }
    if(now->getType() == Fun)
    {
        FunNode* temp = (FunNode*)now;
        int n = temp->getEntity()->getParnum();
        cout << temp->getEntity()->NAME << endl                                                ;
        for(int i = 0; i < n; i++)
            outputASTstruct(temp->sonNode[i], depth+1);;
    }
}
