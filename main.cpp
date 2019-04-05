#include <iostream>
#include <string>
#include "ast.h"
#include "scmath.h"

using namespace std;
using namespace ast;

int main()
{
    string testString = "a+b-c*d/e+f^g-2^a";
    MathFunc test(testString);
    cout << test.diff("a") << endl;
    cout << test.diff("c") << endl;
    cout << test.diff("e") << endl;
    cout << test.diff("f") << endl;
    cout << test.diff("g") << endl;
    cout << endl;

    test.setVal("a", 1);
    cout << test.eval() << endl;
    return 0;
}
