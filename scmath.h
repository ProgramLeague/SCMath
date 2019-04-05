#pragma once
#include "ast.h"
#include "nodetype.h"
#include<sstream>

class MathFunc
{
private:
    BasicNode *funPro;
    Scope* funScope;
public:
    MathFunc(const string &);
    MathFunc(BasicNode *);
    MathFunc(const MathFunc &);
    ~MathFunc();

    void setVal(const string&, const double&);
    void setVal(const string &, BasicNode*);
    const MathFunc eval();
    const MathFunc diff(const string &);

    MathFunc& operator=(const string &);
    MathFunc& operator=(BasicNode *);
    MathFunc& operator=(const MathFunc &);

    friend ostream& operator<<(ostream &, const MathFunc &);
};

void __Simplificate(BasicNode *&);
BasicNode* __Derivation(BasicNode* , const string &);
BasicNode* Derivation(BasicNode*, const string &);

