#include "output.h"

using namespace output;

void output::outputAST(BasicNode* now)
{
    if(now == nullptr)
        return;
    if(now->getType() == Num){
        cout << ((NumNode*)now)->getNum() << ' ';
        return;
    }
    if(now->getType() == Fun){
        FunNode* t = (FunNode*)now;
        Function* l = t->getEntity();
        if(ast::isBinOp(l->NAME)){
            outputAST(t->sonNode[0]);
            cout << l->NAME << ' ';
            outputAST(t->sonNode[1]);
            return;
        }
        cout << l->NAME << '(';
        for(int i = 0 ; i < l->getParnum(); i++){
            outputAST(t->sonNode[i]);
            if(i != l->getParnum() - 1)
                cout << ", ";
        }
        cout << ')';
        return;
    }
}
