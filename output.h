#pragma once
#include "ast.h"
#include <iostream>

namespace output
{
    static bool isBinOp(const string &);
    void outputAST(BasicNode* ,const string& = string(ast::LowestPriority, 1));
    void outputASTstruct(BasicNode*, int = 0);
}
