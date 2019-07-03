#include <iostream>
#include <string>
#include "ast.h"
#include "scmath.h"

using namespace std;
using namespace ast;

int main()
{
    string testString = "1+if(1){1+1}else{2+2;}";
    BasicNode* a = toAST(testString);
    output(a->eval(),cout);
    return 0;
}
/*
 * 目前的建树标准如下：(例子中去掉双引号，所有的输入都忽略空字符'\x20','\t','\n')，其中的LowestPriority为下文的';'
 * 单独语句可以不由';'结尾//例如："1+1" 和 "1+1;" 都正确
 * 多语句一起建树时，每个语句之间必须由';'分开，最后一句话可以没有';'//例如："1+1;2+2"
 * 在if和while后面必须有()作为判断条件，if体、else体、while体必须由"{}"标明
*/
