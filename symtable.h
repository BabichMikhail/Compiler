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
	vector<Symbol*> Table;
public:
	SymTable();
	int Find(string Value);
	Symbol* GetSymbol(string TypeName);
	void Print();
	void Add(Symbol* NewElem);
};

#endif