#include "symbol.h"
#include "syntaxnodes.h"

Symbol::Symbol(DeclSection Section, string Name) : Name(Name), Section(Section){}

SymLabel::SymLabel(string Name) : Symbol(DeclLabel, Name){}
SymType::SymType(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type){}
SymVar::SymVar(string Name, Expr* InitExp, Symbol* Type) : Symbol(DeclVar, Name), InitExp(InitExp), Type(Type){}
SymConst::SymConst(string Name, Expr* InitExp, Symbol* Type) : Symbol(DeclConst, Name), InitExp(InitExp), Type(Type){}

SymDynArray::SymDynArray(Symbol* Type) : Type(Type){}
SymArray::SymArray(Symbol* Type, int Left, int Right) : SymDynArray(Type), Left(Left), Right(Right){}
SymStringType::SymStringType(int Length) : Length(Length){}

bool Symbol::isSame(string Value){
	return Name.length() == Value.length() && _strnicmp(Name.c_str(), Value.c_str(), Name.length()) == 0;
}

DeclSection Symbol::GetSection(){
	return Section;
}

void SymLabel::Print(int Spaces){
	print_indent(Spaces);
	cout << "Label\t" << Name << endl;
}

void SymType::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "Type" << indent << Name << endl;
	if (Type != nullptr)
		Type->Print(Spaces + 1);
}

void SymDynArray::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "DynArray" << indent << Name << endl;
	Type->Print(Spaces);
}

void SymArray::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "Array" << indent << Left << indent << Right << endl;
	Type->Print(Spaces);
}

void SymStringType::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "string" << indent;
	Length != -1 ? cout << Length << endl : cout << endl;
}

void SymVar::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "Var" << indent << Name << endl;
	Type->Print(1);
	if (InitExp != nullptr) {
		InitExp->Print(1);
	}
}

void SymConst::Print(int Spaces){
	if (Spaces > 1) return;
	print_indent(Spaces);
	cout << "Const" << indent << Name << endl;
	if (Type != nullptr) {
		Type->Print(1);
	}
	InitExp->Print(1);
}