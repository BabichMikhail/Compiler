#ifndef CHECK_TYPE_H
#define CHECK_TYPE_H

class SymTable;

#include "syntaxnodes.h"

#define argc_write -1
#define argc_writeln -2

enum MyTypeID {
	TypeID_BadType = -1, TypeID_Integer = 0, TypeID_Double = 1, TypeID_Char = 2, TypeID_Boolean = 3, TypeID_String = 4, TypeID_Array = 5,
	TypeID_DynArray = 6, TypeID_Record = 7, TypeID_Pointer = 8
};

class CmpArguments {
private:
	bool CompareTypes(Symbol* Type_1, Symbol* Type_2);
public:
	bool Compare(Symbol* Sym_1, Symbol* Sym_2);
};

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
	CheckType(SymTable* Table, const Position Pos); /* CheckType(Table, Pos).GetTypeID(const exp) - only for defined type of const declarations  */
	CheckType(SymTable* Table, Expr* Assign, const Position Pos);
	CheckType(SymTable* Table, Symbol* SymType, Expr* Exp, const Position Pos);
	CheckType(SymTable* Table, Expr* Exp_Left, Expr* Exp_Right, const Position Pos);
	CheckType(SymTable* Table, MyTypeID TypeID_1, Expr* Exp, const Position Pos);
	
};
	
#endif