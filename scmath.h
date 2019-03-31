#pragma once
#include "ast.h"
#include "nodetype.h"

void Simplificate(BasicNode *&);
BasicNode* Derivation(BasicNode* , const string &);
