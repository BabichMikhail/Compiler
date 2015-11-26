#ifndef SYMBOL_H
#define SYMBOL_H

#include "syntaxnodes.h"
#include "checktype.h"

class Statement;
class SymTable;

enum DeclSection { DeclNull, DeclConst, DeclVar, DeclLabel, DeclType, DeclFunction, DeclProcedure, DeclRecord };

using namespace std;

class Symbol{
public:
	string Name;
	DeclSection Section;
	virtual bool Compare(Symbol* Sym);
	Symbol(DeclSection Section = DeclNull, string Name = "");
	Symbol(Symbol* Sym);
	bool isSame(string Value);
	virtual void Print(int Spaces){};
	virtual int GetSize() { return 0; }
	virtual void Generate(Asm_Code* Code) {}
	virtual string GenerateName() { return ""; };
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
	string GenerateName();
	int GetSize();
};

class SymPointer : public Symbol {
public:
	Symbol* Type;
	SymPointer(string Name, Symbol* Type);
	void Print(int Spaces);
	int GetSize();
};

class SymIdent : public Symbol {
protected:
	SymIdent(DeclSection Section, string Name, Expr* InitExp, Symbol* Type, ArgState State);
public:
	Expr* InitExp;
	Symbol* Type;
	ArgState State;
	bool isLocal;
	int offset;
	string GenerateName();
	string GetInitList();
	void Generate(Asm_Code* Code);
};

class SymVar : public SymIdent {
public:
	SymVar(string Name, Expr* InitExp, Symbol* Type, ArgState State);
	void Print(int Spaces);
};

class SymConst : public SymIdent {
public:
	SymConst(string Name, Expr* InitExp, Symbol* Type);
	void Print(int Spaces);
};

class SymDynArray : public Symbol{
public:
	Symbol* Type;
	MyTypeID TypeID;
	SymDynArray(Symbol* Type);
	void Print(int Spaces);
	int GetSize();
};

class SymArray : public SymDynArray{
public:
	int Left;
	int Right;
	SymArray(Symbol* Type, int Left, int Right);
	void Print(int Spaces);
	int GetSize();
};

class SymStringType : public Symbol{
public:
	int Length;
	MyTypeID TypeID;
	SymStringType(int Length);
	void Print(int Spaces);
	int GetSize();
};

class SymCall : public Symbol {
protected:
	SymCall(DeclSection Section, string Name, SymTable* Table, Statement* Stmt, int argc);
public:
	SymTable* Table;
	Statement* Stmt;
	int argc;
	string GenerateName();
	void Generate(Asm_Code* Code);
};

class SymFunction : public SymCall {
public:	
	SymFunction(string Name, SymTable* Table, Statement* Stmt, int argc, Symbol* Type);
	void Print(int Spaces);
	Symbol* Type;
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
	int GetSize();
};

#endif