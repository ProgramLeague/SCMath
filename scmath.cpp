#include "scmath.h"

void Simplificate(BasicNode *&now){
      if(now->getType() != Fun)
        return;
    FunNode* tempNow = (FunNode*)now;
    if(tempNow->getEntity()->NAME  ==  "*" || tempNow->getEntity()->NAME == "/"){
        if(tempNow->sonNode[0]->getType() == Fun){
            FunNode* temp = (FunNode*)(tempNow->sonNode[0]);
            if(temp->getEntity()->NAME == "+" || temp->getEntity()->NAME == "-"){
                FunNode* newls = new FunNode(tempNow->getEntity());
                newls->addNode(copyHelp::copyNode(temp->sonNode[0]));
                newls->addNode(copyHelp::copyNode(tempNow->sonNode[1]));
                FunNode* newrs = new FunNode(tempNow->getEntity());
                newrs->addNode(copyHelp::copyNode(temp->sonNode[1]));
                newrs->addNode(copyHelp::copyNode(tempNow->sonNode[1]));
                FunNode* newnow = new FunNode(temp->getEntity());
                newnow->addNode(newls);
                newnow->addNode(newrs);
                delete now;
                now = newnow;
                tempNow = (FunNode*)now;
            }
         }
    }
    if(tempNow->getEntity()->NAME  ==  "*"){
        if(tempNow->sonNode[1]->getType() == Fun){
            FunNode* temp = (FunNode*)(tempNow->sonNode[1]);
            if(temp->getEntity()->NAME == "+" || temp->getEntity()->NAME == "-"){
                FunNode* newls = new FunNode(tempNow->getEntity());
                newls->addNode(copyHelp::copyNode(tempNow->sonNode[0]));
                newls->addNode(copyHelp::copyNode(temp->sonNode[0]));
                FunNode* newrs = new FunNode(tempNow->getEntity());
                newrs->addNode(copyHelp::copyNode(tempNow->sonNode[0]));
                newrs->addNode(copyHelp::copyNode(temp->sonNode[1]));
                FunNode* newnow = new FunNode(temp->getEntity());
                newnow->addNode(newls);
                newnow->addNode(newrs);
                delete now;
                now = newnow;
            }
         }
    }
    Simplificate(now->sonNode[0]);
    Simplificate(now->sonNode[1]);
}
