#include <iostream>
#include <string>
#include "ast.h"
using namespace std;
using namespace ast;


int main()
{
    ast::Init();
    string s = "1+2-3.1+4*2+10/5-2^2";
    BasicNode* ans = ToAST(s);
    return 0;
}
