#include <iostream>
#include <string>
#include "ast.h"
#include "scmath.h"

using namespace std;
using namespace ast;

int main()
{
    string s = "while(1){puts(\"a\")}";
    BasicNode* a = toAST(s);
    BasicNode* b = a->eval();
    cout << a << endl;
    delete a;
    delete b;
    return 0;
}
/*
 * 目前的建树标准如下：(例子中去掉双引号，所有的输入都忽略空字符'\x20','\t','\n')
 * 其中下文的';'为LowestPriority，'$'为ReturnFlag
 * 单独语句可以不由';'结尾//例如："1+1" 和 "1+1;" 都正确
 * 多语句一起建树时，每个语句之间必须由';'分开，最后一句话可以没有';'//例如："1+1;2+2"
 * 在if和while后面必须有()作为判断条件，if体、else体、while体必须由"{}"标明
 * '$'放在句首，标记这段话为返回值
*/
