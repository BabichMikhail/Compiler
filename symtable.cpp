#include "symtable.h"
#include "lexer.h"
#include "checktype.h"

using namespace std;

SymTable::SymTable(SymTable* ParentTable) : Parent(ParentTable) {
	DeclCount = Symbols.size();
}

void SymTable::Add(Symbol* NewElem) {
	Symbols.push_back(NewElem);
}

bool SymTable::Find(string Value) {
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

Symbol* SymTable::FindRequiredSymbol(Expr* Exp, Position Pos) {
	auto Symbols = GetAllSymbols(((ExprIdent*)((ExprFunction*)Exp)->Left)->Sym->Name, Pos);
	for (int i = 0; i < Symbols.size(); ++i) {
		if (Symbols[i]->Section != DeclFunction && Symbols[i]->Section != DeclProcedure) {
			continue;
		}
		if (Symbols[i]->Section == DeclFunction) {
			if (((ExprFunction*)Exp)->Rights.size() != ((SymFunction*)Symbols[i])->argc - 1) {
				continue;
			}
		}
		if (Symbols[i]->Section == DeclProcedure) {
			if (((SymProcedure*)Symbols[i])->argc == argc_writeln || ((SymProcedure*)Symbols[i])->argc == argc_write) {
				return Symbols[i];
			}
			if (((ExprFunction*)Exp)->Rights.size() != ((SymProcedure*)Symbols[i])->argc) {
				continue;
			}
		}
		bool flag = false;
		for (int j = 0; j < ((ExprFunction*)Exp)->Rights.size(); ++j) {
			auto Type = ((SymCall*)Symbols[i])->Table->Symbols[j]->GetType();
			if (Type->Section == DeclRecord && CheckType(this, Position()).GetTypeID(((ExprFunction*)Exp)->Rights[j]) == TypeID_Record){
				continue;
			}
			if (CheckType(this, Position()).GetTypeID(((ExprFunction*)Exp)->Rights[j]) != ((SymType*)Type)->TypeID) {
				flag = true;
			}
		}
		
		if (flag) {
			continue;
		}
		return Symbols[i];
	}
	throw IllegalExpr(Pos);
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
	for (int i = DeclCount; i < Symbols.size(); ++i){ /* No Print STD Type */
		Symbols[i]->Print(Spaces);
		cout << endl;
	}
}

void SymTable::GenerateVariables(Asm_Code* Code) {
	if (Parent == nullptr) {
		for (int i = DeclCount; i < Symbols.size(); ++i) {
			Symbols[i]->Generate(Code);
			if (Symbols[i]->Section == DeclVar || Symbols[i]->Section == DeclConst) {
				((SymIdent*)Symbols[i])->isLocal = false;
			}
		}
	}
}

pair<int, int> SymTable::GenerateLocalVariables(Asm_Code* Code, int last_arg, int first_var, int depth) {
	int offset = 8;
	for (int i = last_arg - 1; i >= 0; --i) {
		if (Symbols[i]->Section == DeclVar) {
			((SymIdent*)Symbols[i])->isLocal = true;
			((SymIdent*)Symbols[i])->offset = offset;
			((SymIdent*)Symbols[i])->depth = depth;
			if (((SymIdent*)Symbols[i])->State == RValue) {
				offset += ((SymIdent*)Symbols[i])->GetSize();
			}
			else {
				offset += 4;
			}
		}
	}
	if (last_arg != first_var) {
		((SymIdent*)Symbols[last_arg])->offset = offset; 
		((SymIdent*)Symbols[last_arg])->isLocal = true;
		((SymIdent*)Symbols[last_arg])->depth = depth;
	}
	int size = 0;
	for (int i = first_var; i < Symbols.size(); ++i) {
		if (Symbols[i]->Section == DeclVar) {
			size -= Symbols[i]->GetType()->GetSize();
			((SymIdent*)Symbols[i])->isLocal = true;
			((SymIdent*)Symbols[i])->offset = size;
			((SymIdent*)Symbols[i])->depth = depth;
		}
		else if (Symbols[i]->Section == DeclProcedure || Symbols[i]->Section == DeclFunction) {
			Symbols[i]->Generate(Code);
		}
	}
	Code->Add(Sub, ESP, to_string(-size));
	for (int i = first_var; i < Symbols.size(); ++i) {
		if (Symbols[i]->Section == DeclVar && ((SymIdent*)Symbols[i])->InitExp != nullptr) {
			ExprAssign* Exp = new ExprAssign(new ExprIdent(Symbols[i], Position()), ((SymIdent*)Symbols[i])->InitExp);
			Exp->Generate(Code);
		}
	}
	return make_pair(-size, offset - 8);
}