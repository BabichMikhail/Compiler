#include "symbol.h"
#include "statement.h"
#include <algorithm>
#include "symtable.h"

#define alignment(size) ((size) / 4 * 4 + ((size) % 4 == 0 ? 0 : 4))

Symbol::Symbol(DeclSection Section, string Name) : Name(Name), Section(Section){ /*transform(this->Name.begin(), this->Name.end(), this->Name.begin(), toupper);*/ }
Symbol::Symbol(Symbol* Sym) : Name(Sym->Name), Section(Sym->Section){}

SymLabel::SymLabel(string Name) : Symbol(DeclLabel, Name){}
SymType::SymType(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type), TypeID(TypeID_BadType){}
SymType::SymType(string Name, MyTypeID TypeID) : Symbol(DeclType, Name), Type(nullptr), TypeID(TypeID){}
SymPointer::SymPointer(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type){}

SymIdent::SymIdent(DeclSection Section, string Name, Expr* InitExp, Symbol* Type, ArgState State) : Symbol(Section, Name), InitExp(InitExp), Type(Type), State(State), isLocal(true) {}
SymVar::SymVar(string Name, Expr* InitExp, Symbol* Type, ArgState State) : SymIdent(DeclVar, Name, InitExp, Type, State){}
SymConst::SymConst(string Name, Expr* InitExp, Symbol* Type, ArgState State) : SymIdent(DeclConst, Name, InitExp, Type, State){}

SymDynArray::SymDynArray(Symbol* Type) : Type(Type), TypeID(TypeID_Array), Symbol(DeclType){}
SymArray::SymArray(Symbol* Type, int Left, int Right) : SymDynArray(Type), Left(Left), Right(Right) {}
SymStringType::SymStringType(int Length) : Length(Length), TypeID(TypeID_String){}

SymCall::SymCall(DeclSection Section, string Name, SymTable* Table, Statement* Stmt, int argc) : Symbol(Section, Name), Table(Table), Stmt(Stmt), argc(argc){}
SymFunction::SymFunction(string Name, SymTable* Table, Statement* Stmt, int argc, Symbol* Type): SymCall(DeclFunction, Name, Table, Stmt, argc), Type(Type){}
SymProcedure::SymProcedure(string Name, SymTable* Table, Statement* Stmt, int argc) : SymCall(DeclProcedure, Name, Table, Stmt, argc){}
SymRecord::SymRecord(SymTable* Table, string Name, int argc) : Symbol(DeclRecord, Name), Table(Table), argc(argc) {}

/*class SymTable {
public:
	vector<Symbol*> Symbols;
	void Print(int Spaces);
	//int GetVariableSize();
	void GenerateLocalVariables(Asm_Code* Code, int first);
};*/

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

int SymPointer::GetSize() {
	return 4;
}

int SymStringType::GetSize() {
	return 4;
}

int SymRecord::GetSize() {
	int size = 0;
	for (int i = 0; i < Table->Symbols.size(); ++i) {
		size += ((SymIdent*)Table->Symbols[i])->GetSize();
	}
	return alignment(size);
}

int SymDynArray::GetSize() {
	return 4;
}

int SymArray::GetSize() {
	return (Right - Left + 1)*Type->GetSize();
}

int SymDynArray::GetLow() {
	return 0;
}

int SymDynArray::GetHigh() {
	return 0;
}

int SymArray::GetLow() {
	return Left;
}

int SymArray::GetHigh() {
	return Right;
}

int SymType::GetSize() {
	switch (TypeID) {
	case TypeID_Integer:
		return 4;
	case TypeID_Double:
		return 8;
	case TypeID_Char:
	case TypeID_Boolean:
		return alignment(1);
	default:
		if (Type == nullptr) {
			return 0;
		}
		return Type->GetSize();
	}
}

string SymType::GenerateName() {
	switch (TypeID) {
 	case TypeID_Integer:
		return "dd";
	case TypeID_Char:
		return "db";
	case TypeID_Boolean:
		return "db";
	}
}

string SymPointer::GenerateName() {
	return "dd";
}

string SymCall::GenerateName() {
	return "c_" + to_string(argc) + "_" + Name + "_" + to_string((int)&argc);
}

void SymCall::Generate(Asm_Code* Code) {
	Asm_Function* Asm_Func = new Asm_Function(GenerateName(), vector<Asm_Cmd*>(), 0);
	Asm_Code* FuncCode = new Asm_Code();
	FuncCode->Fmts = Code->Fmts;
	FuncCode->Add(Push, EBP);
	FuncCode->Add(Mov, EBP, ESP);
	FuncCode->depth = Code->depth + 1;
	auto size = Table->GenerateLocalVariables(FuncCode, Section == DeclFunction ? argc - 1 : argc, argc, FuncCode->depth);
	Stmt->Generate(FuncCode);
	FuncCode->Add(Add, ESP, to_string(size.first));
	FuncCode->Add(Pop, EBP);
	FuncCode->Add(Ret, to_string(size.second));
	for (int i = 0; i < FuncCode->Cmds.size(); ++i) {
		Asm_Func->Cmds.push_back(FuncCode->Cmds[i]);
	}
	for (int i = 0; i < FuncCode->Functions.size(); ++i) {
		Asm_Func->Functions.push_back(FuncCode->Functions[i]);
	}
	Code->Add(Asm_Func);
}

string SymIdent::GenerateName() {
	return "v_" + Name;
}

void SymIdent::Generate(Asm_Code* Code) {
	Code->Add(GenerateName(), Type->GenerateName(), GetInitList());
}

int SymIdent::GetSize() {
	if (Type == nullptr) {
		return 0;
	}
	/*if (State == Var) {
		return 4;
	}*/
	return Type->GetSize();
}

int SymFunction::GetSize() {
	return Type->GetSize();
}

string SymArray::GenerateName() {
	return "times " + to_string(Right - Left + 1) + " " + Type->GenerateName();
}

string SymRecord::GenerateName() {
	return "times " + to_string(GetSize()) + " db";
}

string SymIdent::GetInitList() {
	if (InitExp == nullptr) {
		return "0";
	}
	return InitExp->GenerateInitList();
}

Symbol* SymType::GetType() {
	return Type;
}

Symbol* SymIdent::GetType() {
	return Type;
}

Symbol* SymDynArray::GetType() {
	return Type;
}

Symbol* SymFunction::GetType() {
	return Type;
}

Symbol* SymPointer::GetType() {
	return Type;
}