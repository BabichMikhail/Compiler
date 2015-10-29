#ifndef SYMBOL_H
#define SYMBOL_H

#include "syntaxnodes.h"

enum DeclSection { DeclNull, DeclConst, DeclVar, DeclLabel, DeclType };

using namespace std;

class Symbol{
public:
	string Name;
	DeclSection Section;
	DeclSection GetSection();
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
	SymType(string Name, Symbol* Type);
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
	SymStringType(int Length);
	void Print(int Spaces);
};

#endif