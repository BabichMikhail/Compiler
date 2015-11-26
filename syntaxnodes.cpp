#define _CRT_SECURE_NO_WARNINGS

#include "syntaxnodes.h"
#include <vector>
#include <set>
#include "symbol.h"
#include <algorithm>
#include "symtable.h"
#include "checktype.h"
#include <map>

Expr::Expr(TypeExpr TypeExp) : TypeExp(TypeExp){}

ExprBinOp::ExprBinOp(Expr* Left, Token Op, Expr* Right) : Left(Left), Op(Op), Right(Right), Expr(BinExp){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp), Expr(UnarExp){}

ExprConst::ExprConst(Token Value, TypeExpr TypeExp) : Value(Value), Expr(TypeExp){}
ExprBoolConst::ExprBoolConst(Token Value) : ExprConst(Value, ConstBoolExp){}
ExprIntConst::ExprIntConst(Token Value) : ExprConst(Value, ConstIntExp){}
ExprDoubleConst::ExprDoubleConst(Token Value) : ExprConst(Value, ConstDoubleExp){}
ExprStringConst::ExprStringConst(Token Value) : ExprConst(Value, ConstStringExp){}

ExprIdent::ExprIdent(Symbol* Sym, Position Pos) : Sym(Sym), Expr(VarExp), Pos(Pos){}

ExprArrayIndex::ExprArrayIndex(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(ArrayExp){}
ExprAssign::ExprAssign(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(AssignExp){}
ExprFunction::ExprFunction(Expr* Left, vector<Expr*> Rights) : Left(Left), Rights(Rights), Expr(FunctionExp){}
ExprRecord::ExprRecord(Expr* Left, Symbol* Right) : Left(Left), Right(Right), Expr(RecordExp){}

ExprInitList::ExprInitList(vector<Expr*> List) : Expr(InitExp), List(List) {}

ExprPointer::ExprPointer(Expr* Exp) : Exp(Exp), Expr(PointerExp) {}
ExprDereference::ExprDereference(Expr* Exp) : Exp(Exp), Expr(DereferenceExp) {}

void ExprUnarOp::Print(int Spaces){
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << Op.Source.c_str() << endl;
}

void ExprBinOp::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << Op.Source.c_str() << endl;
	Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	print_indent(Spaces);
	cout << Value.Source.c_str() << endl;
}

void ExprIdent::Print(int Spaces){
	print_indent(Spaces);
	cout << Sym->Name << endl;
}

void ExprAssign::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << ":=" << endl;
	Left->Print(Spaces + 1);
}

void ExprFunction::Print(int Spaces){
	for (int i = 0; i < Rights.size(); ++i){
		Rights[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "()" << endl;
	Left->Print(Spaces + 1);
}

void ExprRecord::Print(int Spaces){
	print_indent(Spaces + 1);
	cout << Right->Name << endl;
	print_indent(Spaces);
	cout << "." << endl;
	Left->Print(Spaces + 1);
}

void ExprArrayIndex::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "[]" << endl;
	Left->Print(Spaces + 1);
}

void ExprInitList::Print(const int Spaces) {
	for (int i = 0; i < List.size(); ++i) {
		List[i]->Print(Spaces);
	}
}

void ExprPointer::Print(const int Spaces) {
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "@" << endl;
} 

void ExprDereference::Print(const int Spaces) {
	print_indent(Spaces);
	cout << "^" << endl;
	Exp->Print(Spaces + 1);
}

void Expr::GetIdentStr(ExpArgList* List){}
	
void ExprBinOp::GetIdentStr(ExpArgList* List){
	Right->GetIdentStr(List);
	Left->GetIdentStr(List);
}

void ExprUnarOp::GetIdentStr(ExpArgList* List){
	Exp->GetIdentStr(List);
}

void ExprConst::GetIdentStr(ExpArgList* List){}

void ExprIdent::GetIdentStr(ExpArgList* List){
	List->Vec.push_back(Sym->Name);
}

void ExprAssign::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void ExprArrayIndex::GetIdentStr(ExpArgList* List){
	Left->GetIdentStr(List);
	Right->GetIdentStr(List);
}

void ExprFunction::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void ExprRecord::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

map<TokenType, AsmOpType> BinOperations = {
	make_pair(TK_PLUS, Add), make_pair(TK_MINUS, Sub), make_pair(TK_XOR, Xor), make_pair(TK_OR, Or), make_pair(TK_MUL, IMul), make_pair(TK_AND, And)
};

void ExprBinOp::Generate(Asm_Code* Code, ArgState State){
	Left->Generate(Code);
	if (Op.Type == TK_SHL || Op.Type == TK_SHR) { /* shl/shr eax, A */
		Code->Add(Pop, EAX);				      /* A - const only */
		if (Right->TypeExp != ConstIntExp) {
			throw ConstIntExpressionExpected(Op.Pos);
		}
		Code->Add(Op.Type == TK_SHL ? Shl : Shr, EAX, ((ExprIntConst*)Right)->Value.Source);
		Code->Add(Push, EAX);
		return;
	}
	Right->Generate(Code);
	Code->Add(Pop, EBX);
	Code->Add(Pop, EAX);
	if (Op.Type == TK_DIV_INT || Op.Type == TK_MOD) {
		Code->Add(Xor, EDX, EDX);
		Code->Add(Div, EBX);
		Op.Type == TK_DIV_INT ? Code->Add(Push, EAX) : Code->Add(Push, EDX);
		return;
	}
	for (auto it = BinOperations.cbegin(); it != BinOperations.cend(); ++it) {
		if ((*it).first == Op.Type) {
			Code->Add((*it).second, EAX, EBX);
			Code->Add(Push, EAX);
			return;
		}
	}
}

void ExprUnarOp::Generate(Asm_Code* Code, ArgState State) {
	switch (Op.Type) {
	case TK_PLUS: 
		return;
	case TK_MINUS:	
		Exp->Generate(Code);
		Code->Add(Pop, EAX);
		Code->Add(Neg, EAX);
		Code->Add(Push, EAX);
		return;
	case TK_NOT: 
		Exp->Generate(Code);
		Code->Add(Pop, EAX);
		Code->Add(Not, EAX);
		Code->Add(Push, EAX);
		return;
	}
}

void ExprIntConst::Generate(Asm_Code* Code, ArgState State) {
	Code->Add(Push, Value.Source);
}

void ExprStringConst::Generate(Asm_Code* Code, ArgState State) {
	int Size = Value.Source.size();
	if (Size == 1) {
		Code->Add(Push, '\'' + Value.Source + '\'');
		return;
	}
	for (int i = 0; i < Size; i += 4) {
		Code->Add(Mov, EAX, '\'' + Value.Source.substr(i, min(4, Size - i)) + '\'');
		Code->Add(Mov, "base_str", i, EAX);
	}
	Code->Add(Mov, EAX, "0x0");
	Code->Add(Mov, "base_str", Size, EAX);
	Code->Add(Push, "base_str");
}

void ExprIdent::Generate(Asm_Code* Code, ArgState State) {
	auto IdenSym = (SymIdent*)Sym;
	switch (IdenSym->State) {
	case Null:
		if (State == Null) {
			Code->Add(Mov, EAX, Sym->GenerateName(), 0);
		}
		else {
			Code->Add(Mov, EAX, Sym->GenerateName());
		}
		break;
	case Var:
	case Const:
	case Out:
		if (IdenSym->isLocal) {
			Code->Add(Mov, EAX, Sym->GenerateName(), 0);
			if (State == Null) {
				Code->Add(Mov, EAX, EAX, 0);
			}
			break;
		}
		Code->Add(Mov, EAX, Sym->GenerateName());
	}
	Code->Add(Push, EAX);
}

void ExprAssign::Generate(Asm_Code* Code, ArgState State) {
	SymIdent* LSym = (SymIdent*)((ExprIdent*)Left)->Sym;
	if (LSym->State == Const) {
		throw VariableIdentifierExpected(((ExprIdent*)Left)->Pos);
	}
	if (LSym->State == Out) {
		LSym->State = Var;
	}
	string Name;
	switch (Left->TypeExp) {
	case VarExp:
		Name = LSym->GenerateName();
		break;
	//case RecordExp:
	//case ArrayExp:
	}
	Right->Generate(Code);
	Code->Add(Pop, EBX);
	if (Right->TypeExp == VarExp && ((SymIdent*)((ExprIdent*)Right)->Sym)->State == Var) {
		Code->Add(Mov, EBX, EBX, 0);
	}
	if (LSym->State == Var) {
		Code->Add(Mov, EAX, Name, 0);
		Code->Add(Mov, EAX, 0, EBX);
		return;
	}
	Code->Add(Mov, Name, 0, EBX);
}

void ExprFunction::Generate(Asm_Code* Code, ArgState State) {
	SymCall* LSym = (SymCall*)((ExprIdent*)Left)->Sym;
	int argc = LSym->argc;
	if (argc >= 0) {
		if (LSym->Section == DeclFunction) {
			Code->Add(Sub, ESP, to_string(((SymIdent*)LSym->Table->Symbols[LSym->argc - 1])->Type->GetSize()));
		}
		for (int i = 0; i < Rights.size(); ++i) {
			Rights[i]->Generate(Code, Rights[i]->TypeExp == VarExp ? ((SymIdent*)LSym->Table->Symbols[LSym->Table->DeclCount + i])->State : Null);
		}
		
		string FuncName = LSym->GenerateName();
		Code->Add(Call, FuncName);
	}
	if (argc == argc_write || argc == argc_writeln){
		vector<MyTypeID> TypeIDexp;
		for (int i = Rights.size() - 1; i >= 0; --i) {
			Rights[i]->Generate(Code);
			TypeIDexp.push_back(CheckType(((SymProcedure*)((ExprIdent*)Left)->Sym)->Table, Position()).GetTypeID(Rights[i]));
		}
		string format = "\'";
		for (int i = 0; i < Rights.size(); ++i) {
			switch (TypeIDexp[TypeIDexp.size() - 1 - i]) {
			case TypeID_Integer:
				format += "%d";
				break;
			case TypeID_Char:
				format += "%c";
				break;
			case TypeID_String:
				format += "%s";
				break;
			}
		}
		format += argc == argc_writeln ? "\', 0xA, 0x0" : "\', 0x0";
		string FormatName = Code->AddFormat(format);

		Code->Add(Push, FormatName); 
		Code->Add(Call, "_printf");
		Code->Add(Add, ESP, to_string(4 * Rights.size() + 4));
	}
}

string ExprConst::GenerateInitList() {
	return Value.Source;
}

string ExprBoolConst::GenerateInitList() {
	return _stricmp(Value.Source.c_str(), "true") == 0 ? "1" : "0";
}

string ExprInitList::GenerateInitList() {
	string Ans = List[0]->GenerateInitList();
	for (int i = 1; List.size(); ++i) {
		Ans += ", " + List[i]->GenerateInitList();
	}
	return Ans;
}

