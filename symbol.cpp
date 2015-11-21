#include "symbol.h"
#include "statement.h"

Symbol::Symbol(DeclSection Section, string Name) : Name(Name), Section(Section){}
Symbol::Symbol(Symbol* Sym) : Name(Sym->Name), Section(Sym->Section){}

SymLabel::SymLabel(string Name) : Symbol(DeclLabel, Name){}
SymType::SymType(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type), TypeID(TypeID_BadType){}
SymType::SymType(string Name, MyTypeID TypeID) : Symbol(DeclType, Name), Type(nullptr), TypeID(TypeID){}
SymPointer::SymPointer(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type), TypeID(TypeID_Pointer){}
SymVar::SymVar(string Name, Expr* InitExp, Symbol* Type) : Symbol(DeclVar, Name), InitExp(InitExp), Type(Type){}
SymConst::SymConst(string Name, Expr* InitExp, Symbol* Type) : Symbol(DeclConst, Name), InitExp(InitExp), Type(Type){}

SymDynArray::SymDynArray(Symbol* Type) : Type(Type), TypeID(TypeID_DynArray){}
SymArray::SymArray(Symbol* Type, int Left, int Right) : SymDynArray(Type), Left(Left), Right(Right) { TypeID = TypeID_Array; }
SymStringType::SymStringType(int Length) : Length(Length), TypeID(TypeID_String){}

SymCall::SymCall(DeclSection Section, string Name, SymTable* Table, Statement* Stmt, int argc) : Symbol(Section, Name), Table(Table), Stmt(Stmt), argc(argc){}
SymFunction::SymFunction(string Name, SymTable* Table, Statement* Stmt, int argc, Symbol* Type): SymCall(DeclFunction, Name, Table, Stmt, argc), Type(Type){}
SymProcedure::SymProcedure(string Name, SymTable* Table, Statement* Stmt, int argc) : SymCall(DeclProcedure, Name, Table, Stmt, argc){}
SymRecord::SymRecord(SymTable* Table, string Name, int argc) : Symbol(DeclRecord, Name), Table(Table), argc(argc) {}

bool Symbol::isSame(string Value){
	return Name.length() == Value.length() && _strnicmp(Name.c_str(), Value.c_str(), Name.length()) == 0;
}

bool Symbol::Compare(Symbol* Sym) {
	return false;
}

void SymLabel::Print(int Spaces){
	print_indent(Spaces);
	cout << "Label\t" << Name << endl;
}

void SymType::Print(int Spaces){
	if (Name.size() == 0) { /* identifier is const and identifier have no TypeName but identifier has auto detect type */
		return;
	}
	print_indent(Spaces);
	cout << "Type" << indent << Name << endl;
	if (Type != nullptr)
		Type->Print(Spaces + 1);
}

void SymPointer::Print(int Spaces) {
	print_indent(Spaces);
	cout << "Type" << indent << Name << endl;
	Type->Print(Spaces + 1);
}

void SymDynArray::Print(int Spaces){
	print_indent(Spaces);
	cout << "DynArray" << indent << Name << endl;
	Type->Print(Spaces);
}

void SymArray::Print(int Spaces){
	print_indent(Spaces);
	cout << "Array" << indent << Left << indent << Right << endl;
	Type->Print(Spaces);
}

void SymStringType::Print(int Spaces){
	print_indent(Spaces);
	cout << "string" << indent;
	Length != -1 ? cout << Length << endl : cout << endl;
}

void SymVar::Print(int Spaces){
	print_indent(Spaces);
	cout << "Var" << indent << Name << endl;
	Type->Print(Spaces + 1);
	if (InitExp != nullptr) {
		InitExp->Print(Spaces + 1);
	}
}

void SymConst::Print(int Spaces){
	print_indent(Spaces);
	cout << "Const" << indent << Name << endl;
	if (Type != nullptr) {
		Type->Print(Spaces + 1);
	}
	InitExp->Print(Spaces + 1);
}

class SymTable {
public:
	void Print(int Spaces);
};

void SymRecord::Print(int Spaces) {
	print_indent(Spaces);
	cout << "Record" << endl;
	this->Table->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "End" << endl;
}

void SymFunction::Print(int Spaces) {
	print_indent(Spaces);
	cout << "Function" << indent << Name << indent << argc << endl;
	Table->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
}

void SymProcedure::Print(int Spaces) {
	print_indent(Spaces);
	cout << "Procedure" << indent << Name << indent << argc << endl;
	Table->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
}