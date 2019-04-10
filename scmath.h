#pragma once
#include "ast.h"
#include "nodetype.h"
#include<sstream>
#include <memory>

typedef matrixNode DataSet;
typedef vector<string> VarSet;
typedef map<string, double> ValWeight;

class MathFunc
{
private:
    BasicNode *funPro = nullptr;
    Scope* funScope = nullptr;
    bool ownerScope = true;
public:
    MathFunc(){}
    MathFunc(const string &, Scope* = nullptr);
    MathFunc(BasicNode *, Scope* = nullptr);
    MathFunc(const MathFunc &, Scope* = nullptr);
    ~MathFunc();

    void setVal(const string &, const double&);
    void setVal(const string &, BasicNode*);
    void changeScope(Scope * = nullptr);
    const MathFunc eval();
    double getNum();
    const MathFunc diff(const string &);
    const ValWeight regress(const DataSet &, const VarSet, int = 500);

    Scope* getScope(){return funScope;}

    MathFunc& operator=(const string &);
    MathFunc& operator=(BasicNode *);
    MathFunc& operator=(const MathFunc &);

    friend ostream& operator<<(ostream &, const MathFunc &);
};

void __Simplificate(BasicNode *&);
BasicNode* __Derivation(BasicNode* , const string &);
BasicNode* Derivation(BasicNode*, const string &);
const ValWeight regression(BasicNode*, const DataSet &, const VarSet, int);
