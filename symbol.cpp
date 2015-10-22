#include "symbol.h"
#include "syntaxnodes.h"

Symbol::Symbol(SymState State, string Name) : State(State), Name(Name){}

SymLabel::SymLabel(SymState State, string Name) : Symbol(State, Name){}

SymType::SymType(SymState State, string NameNewType, Symbol* OldType) : Symbol(State, NameNewType), Type(OldType){}

SymDynArray::SymDynArray(SymState State, string Name, Symbol* ArrType, Expr* Exp_1, Expr* Exp_2) : Symbol(State, Name), ArrType(ArrType){}
SymArray::SymArray(SymState State, string Name, Symbol* ArrType, Expr* Left, Expr* Right) : SymDynArray(State, Name, ArrType), Left(Left), Right(Right){}

SymSimpleType::SymSimpleType(SymState State, string Name, Expr* Exp) : Symbol(State, Name), Exp(Exp){}
SymIntType::SymIntType(SymState State, string Name, Expr* Exp) : SymSimpleType(State, Name, Exp){}
SymRealType::SymRealType(SymState State, string Name, Expr* Exp) : SymSimpleType(State, Name, Exp){}
SymCharType::SymCharType(SymState State, string Name, Expr* Exp) : SymSimpleType(State, Name, Exp){}
SymBoolType::SymBoolType(SymState State, string Name, Expr* Exp) : SymSimpleType(State, Name, Exp){}
SymStringType::SymStringType(SymState State, string Name, vector<Expr*> VecExp, Expr* Exp_Length) : Symbol(State, Name), VecExp(VecExp), Exp_Length(Exp_Length){}
SymTypeID::SymTypeID(SymState State, string Name, vector<Expr*> VecExp, string TypeName) : Symbol(State, Name), VecExp(VecExp), TypeName(TypeName){}

bool Symbol::Find(string Value, SymState AState){
	return Name.length() == Value.length() && _strnicmp(Name.c_str(), Value.c_str(), Name.length()) == 0 && (AState == State_NULL || State == AState);
}

void SymLabel::Print(){
	cout << "Label\t" << Name << endl;
}

void SymType::Print(){
	cout << "Type\t" << Name << endl;
	Type->Print();
}

void SymDynArray::Print(){
	cout << "DynArray\t" << Name << endl;
	ArrType->Print();
}

void SymArray::Print(){
	cout << "Array\t" << Name << endl;
	Left->Print(0);
	Right->Print(0);
	ArrType->Print();
	for (int i = 0; i < VecExp.size(); ++i){
		VecExp[i]->Print(0);
	}
}

void SymIntType::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << "integer" << endl;
	if (Exp != nullptr) 
		Exp->Print(0);
}

void SymRealType::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << "real" << endl;
	if (Exp != nullptr) 
		Exp->Print(0);
}

void SymBoolType::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << "real" << endl;
	if (Exp != nullptr)
		Exp->Print(0);
}

void SymCharType::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << "char" << endl;
	if (Exp != nullptr)
		Exp->Print(0);
}

void SymStringType::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << "string" << endl;
	if (Exp_Length != nullptr) 
		Exp_Length->Print(0);
	for (int i = 0; i < VecExp.size(); ++i)
		VecExp[i]->Print(0);
}

void SymTypeID::Print(){
	switch (State){
	case State_Const: cout << "Const\t" << Name << "\t"; break;
	case State_Var: cout << "Var\t" << Name << "\t"; break;
	default: break;
	}
	cout << TypeName << endl;
	for (int i = 0; i < VecExp.size(); ++i)
		VecExp[i]->Print(0);
}

void Symbol::InitIdent(vector<Expr*> VecExp){}

void SymArray::InitIdent(vector<Expr*> VecExp){
	this->VecExp = VecExp;
}