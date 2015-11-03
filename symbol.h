#ifndef SYMBOL_H
#define SYMBOL_H

#include "syntaxnodes.h"

class Statement;
class SymTable;

enum DeclSection { DeclNull, DeclConst, DeclVar, DeclLabel, DeclType, DeclFunction, DeclProcedure, DeclRecord };

enum MyTypeID {
	TypeID_BadType = -1, TypeID_Integer = 0, TypeID_Double = 1, TypeID_Char = 2, TypeID_Boolean = 3, TypeID_String = 4, TypeID_Array = 5, 
	TypeID_DynArray = 6, TypeID_Record = 7, TypeID_Function = 8, TypeID_Procedure = 9
};

using namespace std;

class Symbol{
public:
	string Name;
	DeclSection Section;
	virtual bool Compare(Symbol* Sym);
	Symbol(DeclSection Section = DeclNull, string Name = "");
	bool isSame(string Value);
	virtual void Print(int Spaces){};
};

class SymLabel : public Symbol{
public:
	SymLabel(string Name);
	void Print(int Spaces);
};

class SymType : public Symbol{
public:
	Symbol* Type;
	MyTypeID TypeID;
	SymType(string Name, Symbol* Type);
	SymType(string Name, MyTypeID TypeID);
	void Print(int Spaces);
};

class SymVar : public Symbol{
public:
	Expr* InitExp;
	Symbol* Type;
	SymVar(string Name, Expr* InitExp, Symbol* Type);
	void Print(int Spaces);
};

class SymConst : public Symbol{
public:
	Expr* InitExp;
	Symbol* Type;
	SymConst(string Name, Expr* InitExp, Symbol* Type);
	void Print(int Spaces);
};

class SymDynArray : public Symbol{
public:
	Symbol* Type;
	MyTypeID TypeID;
	SymDynArray(Symbol* Type);
	void Print(int Spaces);
};

class SymArray : public SymDynArray{
public:
	int Left;
	int Right;
	SymArray(Symbol* Type, int Left, int Right);
	void Print(int Spaces);
};

class SymStringType : public Symbol{
public:
	int Length;
	MyTypeID TypeID;
	SymStringType(int Length);
	void Print(int Spaces);
};

class SymCall : public Symbol {
public:
	SymTable* Table;
	Statement* Stmt;
	int argc;
	SymCall(DeclSection Section, string Name, SymTable* Table, Statement* Stmt, int argc);
};

class SymFunction : public SymCall {
public:	
	Symbol* Type;
	SymFunction(string Name, SymTable* Table, Statement* Stmt, int argc, Symbol* Type);
	void Print(int Spaces);
};

class SymProcedure : public SymCall {
public:
	SymProcedure(string Name, SymTable* Table, Statement* Stmt, int argc);
	void Print(int Spaces);
};

class SymRecord : public Symbol {
public:
	SymTable* Table;
	int argc;
	SymRecord(SymTable* Table, string Name, int argc);
	void Print(int Spaces);
};

#endif