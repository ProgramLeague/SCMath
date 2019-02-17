#pragma once
#include "ast.h"
#include "nodetype.h"

void Simplificate(BasicNode *&now);
BasicNode* Derivation(BasicNode* now, const string &value);
