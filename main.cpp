#include <iostream>
#include <string>
#include "ast.h"
#include "output.h"

using namespace std;

int main()
{
    ast::Init();
    string s = "1+2-3.1+4*2+10/5";
    BasicNode* ans = ast::ToAST(s);
    output::outputAST(ans);
    return 0;
}
