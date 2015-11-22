#define _CRT_SECURE_NO_WARNINGS

#include "syntaxnodes.h"
#include <vector>
#include <set>
#include "symbol.h"
#include <algorithm>
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

ExprVar::ExprVar(Symbol* Sym) : Sym(Sym), Expr(VarExp){}

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

void ExprVar::Print(int Spaces){
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

void ExprVar::GetIdentStr(ExpArgList* List){
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

void ExprBinOp::GetAsmCode(Asm_Code* Code){
	Left->GetAsmCode(Code);
	if (Op.Type == TK_SHL || Op.Type == TK_SHR) { /* shl/shr eax, A */
		Code->Add(Pop, EAX);				      /* A - const only */
		if (Right->TypeExp != ConstIntExp) {
			throw ConstIntExpressionExpected(Op.Pos);
		}
		Code->Add(Op.Type == TK_SHL ? Shl : Shr, EAX, ((ExprIntConst*)Right)->Value.Source);
		Code->Add(Push, EAX);
		return;
	}
	Right->GetAsmCode(Code);
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

void ExprUnarOp::GetAsmCode(Asm_Code* Code) {
	switch (Op.Type) {
	case TK_PLUS: 
		return;
	case TK_MINUS:	
		Code->Add(Pop, EAX);
		Code->Add(Neg, EAX);
		Code->Add(Push, EAX);
		return;
	case TK_NOT: 
		Code->Add(Pop, EAX);
		Code->Add(Not, EAX);
		Code->Add(Push, EAX);
		return;
	}
}

void ExprIntConst::GetAsmCode(Asm_Code* Code) {
	Code->Add(Push, Value.Source);
}

void ExprStringConst::GetAsmCode(Asm_Code* Code) {
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

void ExprFunction::GetAsmCode(Asm_Code* Code) {
	vector<MyTypeID> TypeIDexp;
	for (int i = Rights.size() - 1; i >= 0; --i) {
		vector<Asm_Code*> Ret;
		Rights[i]->GetAsmCode(Code);
		TypeIDexp.push_back(CheckType(((SymRecord*)Left)->Table, Position()).GetTypeID(Rights[i]));
	}
	int argc = ((SymFunction*)((ExprVar*)Left)->Sym)->argc;
	if (argc == argc_write || argc == argc_writeln){
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

