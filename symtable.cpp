#include "symtable.h"
#include "lexer.h"

using namespace std;

SymTable::SymTable(){
	Table.push_back(new SymType("integer", TypeID_Integer));
	Table.push_back(new SymType("double", TypeID_Double));
	Table.push_back(new SymType("char", TypeID_Char));
	Table.push_back(new SymType("boolean", TypeID_Boolean));
	Table.push_back(new SymType("string", TypeID_String));
	Table.push_back(new SymType("array", TypeID_Array));
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

Symbol* SymTable::GetSymbol(string Name, const Position Pos) {
	int idx = Find(Name);
	if (idx == -1) {
		throw IdentifierNotFound(Name, Pos);
	}
	return Table[idx];
}

void SymTable::CheckSymbol(string Name, const Position Pos) {
	if (Find(Name) != -1) {
		throw DuplicateIdentifier(Name, Pos);
	}
}

void SymTable::Print(){
	for (int i = DeclTypeCount; i < Table.size(); ++i){ /* No Print STD Type */
		Table[i]->Print(0);
		cout << endl;
	}
}