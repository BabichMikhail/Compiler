#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string>
#include <vector>
#include <map>
#include "symbol.h"

using namespace std;

class SymTable{
public:
	int DeclCount;

	SymTable* Parent;
	vector<Symbol*> Symbols;
	SymTable(SymTable* ParentTable);

	bool Find(string Value);
	bool FindAll(string Value);
	int FindLocal(string Value);
	Symbol* FindRequiredSymbol(Expr* Exp, Position Pos);
	
	Symbol* GetSymbol(string Name, const Position Pos);
	vector<Symbol*> GetAllSymbols(string Name, const Position Pos);

	void Add(Symbol* NewElem);
	void CheckSymbol(string Name, const Position Pos);
	void Print(int Spaces);
	void GenerateVariables(Asm_Code* Code);
	pair<int, int> GenerateLocalVariables(Asm_Code* Code, int last_arg, int first_var, int depth);
	int Size; 
	OffsetNode *Offsets;
};

#endif