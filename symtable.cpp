#include "symtable.h"
#include "lexer.h"

using namespace std;

SymTable::SymTable(){
	Table.push_back(new SymType("integer", nullptr));
	Table.push_back(new SymType("real", nullptr));
	Table.push_back(new SymType("char", nullptr));
	Table.push_back(new SymType("boolean", nullptr));
	Table.push_back(new SymType("string", nullptr));
	Table.push_back(new SymType("array", nullptr));
	DeclTypeCount = Table.size();
}

void SymTable::Add(Symbol* NewElem){
	Table.push_back(NewElem);
}

int SymTable::Find(string Value){
	for (int i = 0; i < Table.size(); ++i){
		if (Table[i]->isSame(Value)){
			return i;
		}
	}
	return -1;
}

Symbol* SymTable::GetSymbol(string TypeName){
	return Table[Find(TypeName)];
}

void SymTable::Print(){
	for (int i = DeclTypeCount; i < Table.size(); ++i){ /* No Print STD Type */
		Table[i]->Print();
		cout << endl;
	}
}