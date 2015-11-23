#include "symtable.h"
#include "lexer.h"

using namespace std;

SymTable::SymTable(SymTable* ParentTable): Parent(ParentTable){
	DeclTypeCount = Symbols.size();
}

void SymTable::Add(Symbol* NewElem){
	Symbols.push_back(NewElem);
}

bool SymTable::Find(string Value){
	auto TableNow = this;
	do {
		for (int i = 0; i < TableNow->Symbols.size(); ++i) {
			if (TableNow->Symbols[i]->isSame(Value)) {
				return true;
			}
		}
		TableNow = TableNow->Parent;
	} while (TableNow != nullptr);
	return false;
}

bool SymTable::FindAll(string Value) {
	vector<int> Ans;
	auto TableNow = this;
	do {
		for (int i = 0; i < TableNow->Symbols.size(); ++i) {
			if (TableNow->Symbols[i]->isSame(Value)) {
				return true;
			}
		}
		TableNow = TableNow->Parent;
	} while (TableNow != nullptr);
	return false;
}

int SymTable::FindLocal(string Value) {
	for (int i = 0; i < Symbols.size(); ++i) {
		if (Symbols[i]->isSame(Value)) {
			return i;
		}
	}
	return -1;
}

Symbol* SymTable::GetSymbol(string Name, const Position Pos) {
	int idx = -1;
	auto TableNow = this;
	do {
		idx = TableNow->FindLocal(Name);
		if (idx != -1) {
			break;
		}
		TableNow = TableNow->Parent;
	} while (TableNow != nullptr);
	if (idx == -1) {
		throw IdentifierNotFound(Name, Pos);
	}
	return TableNow->Symbols[idx];
}

vector<Symbol*> SymTable::GetAllSymbols(string Name, const Position Pos) {
	vector<Symbol*> Ans;
	auto TableNow = this;
	do {
		for (int i = 0; i < TableNow->Symbols.size(); ++i) {
			if (TableNow->Symbols[i]->isSame(Name)) {
				Ans.push_back(TableNow->Symbols[i]);
			}
		}
		TableNow = TableNow->Parent;
	} while (TableNow != nullptr);
	return Ans;
}

void SymTable::CheckSymbol(string Name, const Position Pos) {
	if (FindLocal(Name) != -1) {
		throw DuplicateIdentifier(Name, Pos);
	}
}

void SymTable::Print(int Spaces){
	for (int i = DeclTypeCount; i < Symbols.size(); ++i){ /* No Print STD Type */
		Symbols[i]->Print(Spaces);
		cout << endl;
	}
}