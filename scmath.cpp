#include "scmath.h"

void Simplificate(BasicNode *&now){
      if(now->getType() != Fun)
        return;
    FunNode* tempNow = (FunNode*)now;
    if(tempNow->getEntity()->NAME  ==  "*" || tempNow->getEntity()->NAME == "/"){
        if(tempNow->sonNode[0]->getType() == Fun){
            FunNode* temp = (FunNode*)(tempNow->sonNode[0]);
            if(temp->getEntity()->NAME == "+" || temp->getEntity()->NAME == "-"){
                FunNode* newnow = new FunNode(temp->getEntity());
                for(int i = 0; i <= 1; i++){
                    FunNode* newson = new FunNode(tempNow->getEntity());
                    newson->addNode(copyHelp::copyNode(temp->sonNode[0]));
                    newson->addNode(copyHelp::copyNode(tempNow->sonNode[1]));
                    newnow->addNode(newson);
                }
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
                FunNode* newnow = new FunNode(temp->getEntity());
                for(int i = 0; i <= 1; i++){
                    FunNode* newson = new FunNode(tempNow->getEntity());
                    newson->addNode(copyHelp::copyNode(tempNow->sonNode[0]));
                    newson->addNode(copyHelp::copyNode(temp->sonNode[0]));
                    newnow->addNode(newson);
                }
                delete now;
                now = newnow;
                tempNow = (FunNode*)now;
            }
         }
    }
    for(int i = 0; i < tempNow->getEntity()->getParnum(); i++)
        Simplificate(now->sonNode[i]);
}
