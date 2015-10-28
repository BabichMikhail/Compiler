#include "symbol.h"
#include "syntaxnodes.h"

Symbol::Symbol(DeclSection Section, string Name) : Name(Name), Section(Section){}

SymLabel::SymLabel(string Name) : Symbol(DeclLabel, Name){}
SymType::SymType(string Name, Symbol* Type) : Symbol(DeclType, Name), Type(Type){}
SymVar::SymVar(string Name, vector<Expr*> VecExp, Symbol* Type) : Symbol(DeclVar, Name), VecExp(VecExp), Type(Type){}
SymConst::SymConst(string Name, vector<Expr*> VecExp, Symbol* Type) : Symbol(DeclConst, Name), VecExp(VecExp), Type(Type){}

SymDynArray::SymDynArray(Symbol* Type) : Type(Type){}
SymArray::SymArray(Symbol* Type, int Left, int Right) : SymDynArray(Type), Left(Left), Right(Right){}
SymStringType::SymStringType(int Length) : Length(Length){}

bool Symbol::isSame(string Value){
	return Name.length() == Value.length() && _strnicmp(Name.c_str(), Value.c_str(), Name.length()) == 0;
}

DeclSection Symbol::GetSection(){
	return Section;
}

void SymLabel::Print(){
	cout << "Label\t" << Name << endl;
}

void SymType::Print(){
	cout << "Type\t" << Name << endl;
	if (Type != nullptr)
		Type->Print();
}

void SymDynArray::Print(){
	cout << "DynArray\t" << Name << endl;
	Type->Print();
}

void SymArray::Print(){
	cout << "Array\t" << Left << "\t" << Right << endl;
	Type->Print();
}

void SymStringType::Print(){
	cout << "string\t";
	Length != -1 ? cout << Length << endl : cout << endl;
}

void SymVar::Print(){
	cout << "Var\t" << Name << endl;
	Type->Print();
	for (int i = 0; i < VecExp.size(); ++i)
		VecExp[i]->Print(0);
}

void SymConst::Print(){
	cout << "Const\t" << Name << endl;
	if (Type != nullptr) {
		Type->Print();
	}
	for (int i = 0; i < VecExp.size(); ++i)
		VecExp[i]->Print(0);
}