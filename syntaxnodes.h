#ifndef SYNTAXNODES_H
#define SYNTAXNODES_H

#include "lexer.h"
#include "errors.h"
#include <vector>
#include <set>
#include "utils.h"

#define indent "   "
#define print_indent(spaces) for (int i = 0; i < spaces; ++i) cout << indent

enum TypeExpr { BinExp, UnarExp, ConstIntExp, ConstDoubleExp, ConstBoolExp, ConstStringExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp, InitExp };

class Expr{
public:
	TypeExpr TypeExp;
	Expr(TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces){};
};

class ExprBinOp : public Expr{
public:
	Expr* Left;
	Token Op;
	Expr* Right;
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprUnarOp : public Expr{
public:
	Token Op;
	Expr* Exp;
	ExprUnarOp(Token Op, Expr* Exp);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprConst : public Expr{
public:
	Token Value;
	ExprConst(Token Value, TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
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

class ExprDoubleConst : public ExprConst{
public:
	ExprDoubleConst(Token Value);
};

class ExprStringConst : public ExprConst{
public:
	ExprStringConst(Token Value);
};

class ExprVar : public Expr{
public:
	Token Var;
	ExprVar(Token Var);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class Assign : public Expr{
public:
	Expr* Left;
	Expr* Right;
	Assign(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ArrayIndex : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ArrayIndex(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class Function : public Expr{
public:
	Expr* Left;
	vector<Expr*> Rights;
	Function(Expr* Left, vector<Expr*> Rights);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class Record : public Expr{
public:
	Expr* Left;
	Expr* Right;
	Record(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class InitList : public Expr {
public:
	vector<Expr*> List;
	InitList(vector<Expr*> List = vector<Expr*>());
	void Print(const int Spaces);
};

#endif