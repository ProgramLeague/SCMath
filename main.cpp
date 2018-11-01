#include <iostream>
#include <string>
#include "ast.h"
#include "output.h"

using namespace std;

int main()
{
    ast::Init();
    string s = "1+2-3*4/5+6*(7+8-9)+sin(0)+a";
    BasicNode* ans = ast::ToAST(s);
    output::outputAST(ans);
    cout << endl;
    output::outputAST(ans ->eval());
    cout << endl;
    output::outputAST(ans);
    return 0;
}
