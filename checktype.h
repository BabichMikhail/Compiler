#ifndef CHECK_TYPE_H
#define CHECK_TYPE_H

#include "symtable.h"
#include "syntaxnodes.h"

class CheckType {
private:
	const Position Pos;
	SymTable* Table;
	MyTypeID GetTypeID(Token TK);
	MyTypeID GetTypeID(TypeExpr TypeExp);
	MyTypeID GetTypeID_BinExp(MyTypeID TypeID_Left, MyTypeID TypeID_Right, TokenType OpType);
	void Check(MyTypeID TypeID_1, MyTypeID TypeID_2);
	bool CanCast(MyTypeID TypeID_1, MyTypeID TypeID_2);
public:
	MyTypeID GetTypeID(Expr* Exp);
	CheckType(SymTable* Table, Expr* Assign, const Position Pos);
	CheckType(SymTable* Table, Symbol* SymType, Expr* Exp, const Position Pos);
	CheckType(SymTable* Table, Expr* Exp_Left, Expr* Exp_Right, const Position Pos);
	CheckType(SymTable* Table, MyTypeID TypeID_1, Expr* Exp, const Position Pos);
	
};
	
#endif