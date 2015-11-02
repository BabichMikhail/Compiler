#ifndef SYMBOL_H
#define SYMBOL_H

#include "syntaxnodes.h"

enum DeclSection { DeclNull, DeclConst, DeclVar, DeclLabel, DeclType };

enum MyTypeID {
	TypeID_BadType = -1, TypeID_Integer = 0, TypeID_Double = 1, TypeID_Char = 2, TypeID_Boolean = 3, TypeID_String = 4, TypeID_Array = 5, 
	TypeID_DynArray = 6 
};

using namespace std;

class Symbol{
public:
	string Name;
	DeclSection Section;
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
	//MyTypeID TypeID;
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

#endif