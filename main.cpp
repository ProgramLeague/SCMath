#include <iostream>
#include <string>
#include "ast.h"
#include "output.h"
#include "scmath.h"

using namespace std;

int main()
{
    /*
    string s = "1+2-3*4/5*(7+8-b)+sin(0)+a + 2 ^    5 ";
    BasicNode* ans = ast::ToAST(s);
    output::outputAST(ans);
    cout << endl;
    output::outputAST(ans ->eval());
    cout << endl;
    output::outputAST(ans);
    */
    string s ="(1+2)*(3+4)+(a+b)/(c+d)";
    BasicNode* ans = ast::ToAST(s);
    output::outputAST(ans);
    cout << endl;
    Simplificate(ans);
    output::outputAST(ans);
    cout << endl;
    return 0;
}
