#ifndef SYNTAXNODES_H
#define SYNTAXNODES_H

#include "lexer.h"
#include "errors.h"
#include <vector>
#include <set>

enum TypeExpr { BinExp, UnarExp, ConstExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp };

class Expr{
public:
	TypeExpr TypeExp;
	Expr(TypeExpr TypeExp);
	virtual void Print(const int Spaces){};
};

class ExprBinOp : public Expr{
private:
	Expr* Left;
	Token Op;
	Expr* Right;
public:
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void Print(const int Spaces);
};

class ExprUnarOp : public Expr{
private:
	Token Op;
	Expr* Exp;
public:
	ExprUnarOp(Token Op, Expr* Exp);
	void Print(const int Spaces);
};

class ExprConst : public Expr{
private:
	Token Value;
public:
	ExprConst(Token Value);
	virtual void Print(const int Spaces);
};

class ExprBoolConst : public ExprConst{
public:
	ExprBoolConst(Token Value);
};

class ExprIntConst : public ExprConst{
public:
	ExprIntConst(Token Value);
};

class ExprRealConst : public ExprConst{
public:
	ExprRealConst(Token Value);
};

class ExprVar : public Expr{
private:
	Token Var;
public:
	ExprVar(Token Var);
	void Print(const int Spaces);
};

class Assign : public Expr{
private:
	Expr* Left;
	Expr* Right;
public:
	Assign(Expr* Left, Expr* Right);
	void Print(const int Spaces);
};

class ArrayIndex : public Expr{
private:
	Expr* Left;
	Expr* Right;
public:
	ArrayIndex(Expr* Left, Expr* Right);
	void Print(const int Spaces);
};

class Function : public Expr{
private:
	Expr* Left;
	vector<Expr*> Rights;
public:
	Function(Expr* Left, vector<Expr*> Rights);
	void Print(const int Spaces);
};

class Record : public Expr{
private:
	Expr* Left;
	Expr* Right;
public:
	Record(Expr* Left, Expr* Right);
	void Print(const int Spaces);
};

#endif