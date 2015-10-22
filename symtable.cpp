#include "symtable.h"
#include "lexer.h"

using namespace std;

void SymTable::Add(Symbol* NewElem){
	Table.push_back(NewElem);
}

int SymTable::Find(string Value, SymState State){
	for (int i = 0; i < Table.size(); ++i){
		if (Table[i]->Find(Value, State)){
			return i;
		}
	}
	return -1;
}

void SymTable::Print(){
	for (int i = 0; i < Table.size(); ++i){
		Table[i]->Print();
		cout << endl;
	}
}