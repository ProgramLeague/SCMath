#pragma once
#include "ast.h"
#include <iostream>

namespace output
{
    static bool isBinOp(const string &c);
    void outputAST(BasicNode* ,int = 0);
}
