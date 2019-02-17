#include <iostream>
#include <string>
#include "ast.h"
#include "output.h"
#include "scmath.h"

using namespace std;

int main()
{

    string TestBuildTree = "1-(2+3)-(4-5)";
    BasicNode* ansTestBuildTree = ast::ToAST(TestBuildTree);
    BasicNode* ansAfterEval;
    output::outputASTstruct(ansTestBuildTree);
    output::outputAST(ansTestBuildTree);
    cout << endl;
    output::outputAST(ansAfterEval = ansTestBuildTree ->eval());
    cout << endl;
    delete ansAfterEval;
    output::outputAST(ansTestBuildTree);
    cout << endl << endl;
    delete ansTestBuildTree;

    string TestSimplificate ="(1+2)*(3+4)+(a+b)/(c+d)";
    BasicNode* ansTestSimplificate = ast::ToAST(TestSimplificate);
    output::outputAST(ansTestSimplificate);
    cout << endl;
    Simplificate(ansTestSimplificate);
    output::outputAST(ansTestSimplificate);
    cout << endl << endl;
    delete ansTestSimplificate;

    string TestDerivation = "a+b-c*d/e+f^g-2^a";
    BasicNode* ansTestDerivation = ast::ToAST(TestDerivation);
    BasicNode* ansAfterDerivation;
    output::outputAST(ansTestDerivation);
    cout << endl;
    ansAfterDerivation = Derivation(ansTestDerivation, "a");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl;
    delete ansAfterDerivation;
    ansAfterDerivation = Derivation(ansTestDerivation, "c");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl;
    delete ansAfterDerivation;
    ansAfterDerivation = Derivation(ansTestDerivation, "e");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl;
    delete ansAfterDerivation;
    ansAfterDerivation = Derivation(ansTestDerivation, "f");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl;
    delete ansAfterDerivation;
    ansAfterDerivation = Derivation(ansTestDerivation, "g");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl << endl;
    delete ansAfterDerivation;
    delete ansTestDerivation;

    TestDerivation = "sin(x)*cos(x)";
    ansTestDerivation = ast::ToAST(TestDerivation);
    output::outputAST(ansTestDerivation);
    cout << endl;
    ansAfterDerivation = Derivation(ansTestDerivation, "x");
    Simplificate(ansAfterDerivation);
    output::outputAST(ansAfterDerivation);
    cout << endl << endl;
    return 0;
}
