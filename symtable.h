#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string>
#include <vector>
#include "symbol.h"

using namespace std;

typedef struct {
	Symbol Sym;
	string Name;
} Field;

class SymTable{
private:
	int DeclTypeCount;
public:
	SymTable* Parent;
	vector<Symbol*> Symbols;
	SymTable(SymTable* ParentTable);

	int Find(string Value);
	int FindLocal(string Value);
	vector<int> FindAll(string Value);
	
	Symbol* GetSymbol(string Name, const Position Pos);
	vector<Symbol*> GetAllSymbols(string Name, const Position Pos);

	void CheckSymbol(string Name, const Position Pos);
	void Add(Symbol* NewElem);
	void Print(int Spaces);

};

#endif