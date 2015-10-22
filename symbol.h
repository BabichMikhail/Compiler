#ifndef SYMBOL_H
#define SYMBOL_H

#include "syntaxnodes.h"

using namespace std;

enum SymState { State_NULL, State_Var, State_Const, State_Type, State_Label };

class Symbol{
public:
	string Name;
	SymState State;
	Symbol(SymState State, string Name = "");
	virtual void InitIdent(vector<Expr*> VecExp);
	bool Find(string Value, const SymState State = State_NULL);
	virtual void Print(){};
};

class SymLabel : public Symbol{
public:
	SymLabel(SymState State, string Name);
	void Print();
};

/*class SymFunc : public Symbol{
public:
	vector<Symbol> Args;
	Symbol* Ret;
	SymFunc(SymState State, string Name);
	void Print();
};*/

/*class SymProc : public Symbol{
	vector<Symbol> Args;
	SymProc(SymState State, string Name);
	void Print();
};*/

class SymType : public Symbol{
public:
	Symbol* Type;
	SymType(SymState State, string NameNewType, Symbol* OldType);
	void Print();
};

class SymDynArray : public Symbol{
public:
	Symbol* ArrType;
	SymDynArray(SymState State, string Name, Symbol* ArrType, Expr* Exp_1 = nullptr, Expr* Exp_2 = nullptr); /* for optimize parse */
	void Print();	/* Exp_1 == nullptr && Exp_2 == nullptr - always (else SymArray) */
};

class SymArray : public SymDynArray{
public:
	Expr* Left;
	Expr* Right;
	vector<Expr*> VecExp;
	void InitIdent(vector<Expr*> VecExp);
	vector<Expr*> Init_Exp;
	SymArray(SymState State, string Name, Symbol* ArrType, Expr* Left, Expr* Right);
	void Print();
};

class SymSimpleType : public Symbol{
public:
	Expr* Exp;
	SymSimpleType(SymState State, string Name, Expr* Exp);
};

class SymIntType : public SymSimpleType{
public:
	SymIntType(SymState State, string Name = "", Expr* Exp = nullptr);
	void Print();
};

class SymRealType : public SymSimpleType{
public:
	SymRealType(SymState State, string Name = "", Expr* Exp = nullptr);
	void Print();
};

class SymBoolType : public SymSimpleType{
public:
	SymBoolType(SymState State, string Name = "", Expr* Exp = nullptr);
	void Print();
};

class SymCharType : public SymSimpleType{
public:
	SymCharType(SymState State, string Name = "", Expr* Exp = nullptr);
	void Print();
};

class SymStringType : public Symbol{
public:
	vector<Expr*> VecExp;
	Expr* Exp_Length;
	SymStringType(SymState State, string Name = "", vector<Expr*> VecExp = vector<Expr*>(), Expr* Length = nullptr);
	void Print();
};

class SymTypeID : public Symbol{
public:
	vector<Expr*> VecExp;
	string TypeName;
	SymTypeID(SymState State, string Name, vector<Expr*> VecExp, string TypeName);
	void Print();
};


#endif