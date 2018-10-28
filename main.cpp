#include <iostream>
#include <string>
#include "ast.h"
#include "output.h"

using namespace std;

int main()
{
    ast::Init();
    string s = "1+2-3*4/5+6*(7+8-9)+sin(0)";
    BasicNode* ans = ast::ToAST(s);
    output::outputAST(ans);
    return 0;
}
