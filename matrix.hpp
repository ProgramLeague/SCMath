#pragma once
#include "nodetype.h"
#include <math.h>
using namespace std;

class vectorNode : public BasicNode
{
private:
    unsigned int l;

public:
    //SCMath兼容
    virtual int getType() {return Vector;}
    virtual void addNode(BasicNode*) {throw addSonExcep(Vector);}
    virtual BasicNode* eval() {return this;}

    //原生功能
    double *v;

    vectorNode(unsigned int l)
    {
        this->l = l;
        this->v = new double[l];
        for (unsigned int i = 0; i < l; i++)
            this->v[i] = 0;
    }

    vectorNode(const vectorNode& m2) : BasicNode(m2)
    {
        this->l = m2.l;
        this->v = new double[l];
        for (unsigned int i = 0; i < l; i++)
            this->v[i] = m2.v[i];
    }

    double dot(vectorNode &v2)
    {
        if (v2.l != this->l)
            throw string("两个向量维度不对不能相乘");
        else
        {
            double result = 0;
            for (unsigned int i = 0; i < l; i++)
                result += this->v[i] * v2.v[i];
            return result;
        }
    }

    vectorNode mul(double n)
    {
        vectorNode result(this->l);
        for (unsigned int i = 0; i < l; i++)
            result.v[i]=this->v[i]*n;
        return result;
    }

    vectorNode add(vectorNode v2)
    {
        if(v2.l!=this->l)
            throw string("两个向量维度不对不能相加");
        else
        {
            vectorNode result(this->l);
            for (unsigned int i = 0; i < l; i++)
                result.v[i]=this->v[i]+v2.v[i];
            return result;
        }
    }

    void output()
    {
        for (unsigned int i = 0; i < l; i++)
        {
            printf("%g ", v[i]);
        }
        printf("\n");
    }

    ~vectorNode() { delete[]v; }
    unsigned int getl() { return l; }
};


class matrixNode : public BasicNode
{
private:
    unsigned int r;
    unsigned int c;

    void malloc()
    {
        this->m = new double*[r]; //给第一维分配空间
        for (unsigned int i = 0; i < r; i++)
            this->m[i] = new double[c]; //给第二维分配空间
    }

public:
    //SCMath兼容
    virtual int getType() {return Matrix;}
    virtual void addNode(BasicNode*) {throw addSonExcep(Matrix);}
    virtual BasicNode* eval() {return this;}

    //原生功能
    double **m;

    matrixNode(unsigned int r, unsigned int c)
    {
        this->r = r;
        this->c = c;
        this->malloc();
        //初始化为零矩阵
        for (unsigned int i = 0; i < r; i++)
        {
            for (unsigned int j = 0; j < c; j++)
                this->m[i][j] = 0;
        }
    }

    matrixNode(const matrixNode& m2) : BasicNode(m2)
    {
        this->r = m2.r;
        this->c = m2.c;
        this->malloc();
        for (unsigned int i = 0; i < r; i++)
        {
            for (unsigned int j = 0; j < c; j++)
                this->m[i][j] = m2.m[i][j];
        }
    }

    ~matrixNode()
    {
        for (unsigned int i = 0; i < r; i++)
            delete[] m[i];
        delete[] m;
    }

    unsigned int getr() { return r; }
    unsigned int getc() { return c; }

    vectorNode getRVector(unsigned int rn)
    {
        vectorNode result = vectorNode(this->c);
        for (unsigned int i = 0; i < this->c; i++)
            result.v[i] = this->m[rn][i];
        return result;
    }

    vectorNode getCVector(unsigned int cn)
    {
        vectorNode result = vectorNode(this->r);
        for (unsigned int i = 0; i < this->r; i++)
            result.v[i] = this->m[i][cn];
        return result;
    }

    void setRVector(vectorNode v, unsigned int rn)
    {
        if(v.getl()!=this->c)
            throw string("赋值的向量维度与矩阵行列长度不匹配");
        else
        {
            for (unsigned int i = 0; i < this->c; i++)
                this->m[rn][i]=v.v[i];
        }
    }

    void setCVector(vectorNode v, unsigned int cn)
    {
        if(v.getl()!=this->r)
            throw string("赋值的向量维度与矩阵行列长度不匹配");
        else
        {
            for (unsigned int i = 0; i < this->r; i++)
                this->m[i][cn]=v.v[i];
        }
    }

    double algCofactor(unsigned int i,unsigned int j)
    {
        matrixNode mb = matrixNode(r - 1, r - 1);
        for (unsigned int r = 0; r < this->r; r++)
        {
            for (unsigned int c = 0; c < this->c; c++)
            {
                if(c==j || r==i)
                    continue;

                int subR,subC;
                if (c > j)
                    subC = c - 1;
                else
                    subC = c;
                if(r > i)
                    subR = r-1;
                else
                    subR = r;

                mb.m[subR][subC] = this->m[r][c];
            }
        }
        return pow(-1, i + j) * mb.det();
    }

    double det()
    {
        if (this->r != this->c)
            throw string("只有方阵才能求行列式");
        else if (this->r == 1)
            return this->m[0][0];
        else
        {
            double result = 0;
            //得到从当前矩阵中划去第0行和第j列的所有元素后得到的矩阵
            for (unsigned int j = 0; j < this->r; j++)
            {
                result += this->algCofactor(0,j) * this->m[0][j];
            }
            return result;
        }
    }

    matrixNode adjoint()
    {
        if (this->r != this->c)
            throw string("只有方阵才能求伴随矩阵");
        else
        {
            matrixNode result = matrixNode(this->r,this->c);
            for (unsigned int i = 0; i < this->r; i++)
            {
                for (unsigned int j = 0; j < this->c; j++)
                    result.m[i][j]=this->algCofactor(j,i);
            }
            return result;
        }
    }

    matrixNode inv()
    {
        double d=this->det();
        return this->adjoint().mul(1/d);
    }

    matrixNode dot(matrixNode &m2)
    {
        if (this->c != m2.r) //如果矩阵A的列数不等于矩阵B的行数……
            throw string("两个矩阵的维度不能相乘");
        else
        {
            matrixNode result = matrixNode(this->r, m2.c);
            for (unsigned int i = 0; i < this->r; i++)
            {
                vectorNode v1 = this->getRVector(i);
                for (unsigned int j = 0; j < m2.c; j++)
                {
                    vectorNode v2 = m2.getCVector(j);
                    result.m[i][j] = v1.dot(v2);
                }
            }
            return result;
        }
    }

    matrixNode mul(double n)
    {
        matrixNode result(this->r,this->c);
        for (unsigned int i = 0; i < this->r; i++)
            for (unsigned int j = 0; j < this->c; j++)
                result.m[i][j]=this->m[i][j]*n;
        return result;
    }

    matrixNode add(matrixNode m2)
    {
        if(m2.c==this->c&&m2.r==this->r)
        {
            matrixNode result(this->r,this->c);
            for (unsigned int i = 0; i < this->r; i++)
                for (unsigned int j = 0; j < this->c; j++)
                    result.m[i][j]=this->m[i][j]+m2.m[i][j];
            return result;
        }
        else
            throw string("两个矩阵维度不对不能相加");
    }

    void output()
    {
        for (unsigned int i = 0; i < r; i++)
        {
            for (unsigned int j = 0; j < c; j++)
            {
                printf("%g ", m[i][j]);
            }
            printf("\n\t");
        }
    }

    static vectorNode solve(matrixNode m, vectorNode v) //克拉默法则求解
    {
        double D = m.det();
        vectorNode result = vectorNode(v.getl());
        for (unsigned int i = 0; i < v.getl(); i++) //逐行替换并计算行列式，目前替换第i列
        {
            matrixNode mb = matrixNode(m);
            for (unsigned int j = 0; j < v.getl(); j++) //替换第i列的第j个元素
                mb.m[j][i] = v.v[j];
            result.v[i] = mb.det() / D;
        }
        return result;
    }
};

