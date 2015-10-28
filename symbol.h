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
	virtual void Print(){};
};

class SymLabel : public Symbol{
public:
	SymLabel(string Name);
	void Print();
};

class SymType : public Symbol{
public:
	Symbol* Type;
	SymType(string Name, Symbol* Type);
	void Print();
};

class SymVar : public Symbol{
public:
	vector<Expr*> VecExp;
	Symbol* Type;
	SymVar(string Name, vector<Expr*> VecExp, Symbol* Type);
	void Print();
};

class SymConst : public Symbol{
public:
	vector<Expr*> VecExp;
	Symbol* Type;
	SymConst(string Name, vector<Expr*> VecExp, Symbol* Type);
	void Print();
};

class SymDynArray : public Symbol{
public:
	Symbol* Type;
	SymDynArray(Symbol* Type);
	void Print();
};

class SymArray : public SymDynArray{
public:
	int Left;
	int Right;
	SymArray(Symbol* Type, int Left, int Right);
	void Print();
};

class SymStringType : public Symbol{
public:
	int Length;
	SymStringType(int Length);
	void Print();
};

#endif