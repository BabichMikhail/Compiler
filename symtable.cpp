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
		for (auto it = TableNow->Symbols.begin(); it < TableNow->Symbols.end(); ++it) {
			if ((*it)->isSame(Value)) {
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
		for (auto it = TableNow->Symbols.begin(); it < TableNow->Symbols.end(); ++it) {
			if ((*it)->isSame(Value)) {
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
	for (auto it = Symbols.begin(); it < Symbols.end(); ++it) {
		if ((*it)->Section != DeclFunction && (*it)->Section != DeclProcedure) {
			continue;
		}
		if ((*it)->Section == DeclFunction) {
			if (((ExprFunction*)Exp)->Args.size() != ((SymFunction*)*it)->argc - 1) {
				continue;
			}
		}
		if ((*it)->Section == DeclProcedure) {
			if (((SymProcedure*)*it)->argc == argc_writeln || ((SymProcedure*)*it)->argc == argc_write) {
				return *it;
			}
			if (((ExprFunction*)Exp)->Args.size() != ((SymProcedure*)*it)->argc) {
				continue;
			}
		}
		bool flag = false;
		for (auto it_arg = ((ExprFunction*)Exp)->Args.begin(); it_arg < ((ExprFunction*)Exp)->Args.end(); ++it_arg) {
			auto Type = ((SymCall*)*it)->Table->Symbols[it_arg - ((ExprFunction*)Exp)->Args.begin()]->GetType();
			if (Type->Section == DeclRecord && CheckType(this, Position()).GetTypeID(*it_arg) == TypeID_Record){
				continue;
			}
			if (CheckType(this, Position()).GetTypeID(*it_arg) != ((SymType*)Type)->TypeID) {
				flag = true;
			}
		}
		
		if (flag) {
			continue;
		}
		return *it;
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
		for (auto it = TableNow->Symbols.begin(); it < TableNow->Symbols.end(); ++it) {
			if ((*it)->isSame(Name)) {
				Ans.push_back(*it);
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
	for (auto it = Symbols.begin() + DeclCount; it < Symbols.end(); ++it) { /* No Print STD Type */
		(*it)->Print(Spaces);
		cout << endl;
	}
}

void SymTable::GenerateVariables(Asm_Code* Code) {
	if (Parent == nullptr) {
		for (auto it = Symbols.begin() + DeclCount; it < Symbols.end(); ++it) {
			(*it)->Generate(Code);
			if ((*it)->Section == DeclVar || (*it)->Section == DeclConst) {
				((SymIdent*)(*it))->isLocal = false;
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
	for (auto it = Symbols.begin() + first_var; it < Symbols.end(); ++it) {
		if ((*it)->Section == DeclVar) {
			size -= (*it)->GetType()->GetSize();
			((SymIdent*)*it)->isLocal = true;
			((SymIdent*)*it)->offset = size;
			((SymIdent*)*it)->depth = depth;
		}
		else if ((*it)->Section == DeclProcedure || (*it)->Section == DeclFunction) {
			(*it)->Generate(Code);
		}
	}
	Code->Add(Sub, ESP, to_string(-size));
	for (auto it = Symbols.begin() + first_var; it < Symbols.end(); ++it) {
		if ((*it)->Section == DeclVar && ((SymIdent*)*it)->InitExp != nullptr) {
			ExprAssign* Exp = new ExprAssign(new ExprIdent(*it, Position()), ((SymIdent*)*it)->InitExp);
			Exp->Generate(Code);
		}
	}
	return make_pair(-size, offset - 8);
}