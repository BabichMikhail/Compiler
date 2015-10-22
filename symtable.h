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
	vector<Symbol*> Table;
public:
	int Find(string Value, SymState State = State_NULL);
	void Print();
	void Add(Symbol* NewElem);
};

#endif