#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum PState{
	St_Parse_Expr, St_Parse_Simple_Expr, St_Parse_Term, St_Parse_Factor
};

class Expr{	
public:
	Expr();
	virtual void Print(int Spaces){};
};

class ExprBinOp : Expr{
private:
	Expr* Left;
	Token Op;
	Expr* Right;
public:
	ExprBinOp(Token Op, Expr* L, Expr* R);
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
	Token Val;
public:
	ExprConst(Token Val);
	virtual void Print(int Spaces);
};

class ExprBoolConst : ExprConst{
public:
	ExprBoolConst(Token Val);
};

class ExprIntConst : ExprConst{
public:
	ExprIntConst(Token Val);
};

class ExprRealConst : ExprConst{
public:
	ExprRealConst(Token Val);
};

class ExprVar : ExprConst{
public:
	ExprVar(Token Var);
};

class Parser{
private:
	Lexer Lex;
	Expr* Exp;
	PState State;
public:
	Parser(const char* filename);
	Expr* ParseByParam(PState State);
	void Print();
};

#endif