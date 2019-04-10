#include <iostream>
#include <string>
#include "ast.h"
#include "scmath.h"

using namespace std;
using namespace ast;

int main()
{
    string testString = "a+b-c*d/e+f^g-2^a";
    MathFunc test = testString;
    cout << test.diff("a") << endl;
    cout << test.diff("c") << endl;
    cout << test.diff("e") << endl;
    cout << test.diff("f") << endl;
    cout << test.diff("g") << endl;
    test.setVal("a", 1);
    cout << test.eval() << endl;
    test.setVal("a", 2);
    cout << test.eval() << endl;
    cout << endl;

    string testRegression = "a*x+b";
    test = testRegression;
    cout << testRegression << endl;
    DataSet data(2, 2);
    data.m[0][0] = 2; data.m[0][1] = 2;//(2, 2)
    data.m[1][0] = 3; data.m[1][1] = 5;//(3, 5)
    VarSet varlist;
    varlist.push_back("x");
    ValWeight weight = test.regress(data, varlist, 10000);
    for(auto &i : weight)
        cout << i.first << '\t' << i.second << endl;
    return 0;
}
