#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>

using namespace std;

enum PState{
	St_Parse_Expr, St_Parse_Simple_Expr, St_Parse_Term, St_Parse_Factor, St_Assign, St_Record, St_ArrayIndex, St_Function
};

/* Expr */ 
class Expr{
public:
	int IndType;
	Expr(int IT);
	virtual void Print(int Spaces){};
};

class ExprBinOp : Expr{
private:
	Expr* Left;
	Token Op;
	Expr* Right;
public:
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void Print(int Spaces);
};

class ExprUnarOp : Expr{
private:
	Token Op;
	Expr* Exp;
public:
	ExprUnarOp(Token Op, Expr* Exp);
	void Print(int Spaces);
};

class ExprConst : Expr{
private:
	Token Value;
public:
	ExprConst(Token Value);
	virtual void Print(int Spaces);
};

class ExprBoolConst : ExprConst{
public:
	ExprBoolConst(Token Value);
};

class ExprIntConst : ExprConst{
public:
	ExprIntConst(Token Value);
};

class ExprRealConst : ExprConst{
public:
	ExprRealConst(Token Value);
};

class ExprVar : Expr{
private:
	Token Var;
public:
	ExprVar(Token Var);
	void Print(int Spaces);
};

/* Assign */
class Assign : Expr{
private:
	ExprVar* Left;
	Expr* Right;
public:
	Assign(ExprVar* Left, Expr* Right);
	void Print(int Spaces);
};

/* Array Index */
class ArrayIndex : Expr{
private:
	ExprVar* Left;
	Expr* Right;
public:
	ArrayIndex(ExprVar* Left, Expr* Right);
	void Print(int Spaces);
};

/* Function */
class Function : Expr{
private:
	ExprVar* Left;
	vector<Expr*> Rights;
public:
	Function(ExprVar* Left, vector<Expr*> Rights);
	void Print(int Spaces);
};

/* Record */
class Record : Expr{
private:
	Expr* Left;
	ExprVar* Right;
public:
	Record(Expr* Left, ExprVar* Right);
	void Print(int Spaces);
};

class Parser{
private:
	Lexer Lex;
	Expr* Exp;
	PState State;
public:
	Parser(const char* filename);
	Expr* ParseExprByParam(PState State);
	Expr* ParseIdentifier();
	void Print();
};

#endif