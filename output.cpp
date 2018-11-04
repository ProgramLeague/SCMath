#include "output.h"

using namespace output;

bool output::isBinOp(const string &c)
{
    return c == "+" || c == "-" || c == "*" || c == "/" || c == "^";
}


void output::outputAST(BasicNode* now, int fatherpriority)
{
    if(now == nullptr)
        return;
    if(now->getType() == Num){
        cout << ((NumNode*)now)->getNum();
        return;
    }
    if(now->getType() == Fun){
        FunNode* t = (FunNode*)now;
        Function* l = t->getEntity();
        if(output::isBinOp(l->NAME)){
            if(ast::BinOpPriority[l->NAME] < fatherpriority)
                cout << '(';
            outputAST(t->sonNode[0], ast::BinOpPriority[l->NAME]);
            cout << l->NAME;
            outputAST(t->sonNode[1], ast::BinOpPriority[l->NAME]);
            if(ast::BinOpPriority[l->NAME] < fatherpriority)
                cout << ')';
            return;
        }
        cout << l->NAME << '(';
        for(int i = 0 ; i < l->getParnum(); i++){
            outputAST(t->sonNode[i], 0);
            if(i != l->getParnum() - 1)
                cout << ",";
        }
        cout << ')';
        return;
    }
    if(now->getType() == Var){
        cout << ((VarNode*)now)->NAME;
    }
}
