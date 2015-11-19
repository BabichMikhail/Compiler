#define _CRT_SECURE_NO_WARNINGS

#include "syntaxnodes.h"
#include <vector>
#include <set>
#include "asmgenerator.h"
#include <algorithm>

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
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
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

set<pair<TokenType, AsmOp>> BinOperations = {
	make_pair(TK_PLUS, AsmAdd), make_pair(TK_MINUS, AsmSub), make_pair(TK_XOR, AsmXor), make_pair(TK_OR, AsmOr),
	make_pair(TK_MUL, AsmIMul), make_pair(TK_AND, AsmAnd), make_pair(TK_SHL, AsmShl), make_pair(TK_SHR, AsmShr)
};

vector<Asm_Code*> ExprBinOp::GetAsmCode(){
	vector<Asm_Code*> Ans(Left->GetAsmCode());
	auto _Right(Right->GetAsmCode());
	for (int i = 0; i < _Right.size(); ++i) {
		Ans.push_back(_Right[i]);
	}
	Ans.push_back(new Asm_Unar_Cmd(AsmPop, new Asm_Registr(AsmEBX)));
	Ans.push_back(new Asm_Unar_Cmd(AsmPop, new Asm_Registr(AsmEAX)));
	if (Op.Type == TK_DIV_INT || Op.Type == TK_MOD) {
		Ans.push_back(new Asm_Bin_Cmd(AsmXor, new Asm_Registr(AsmEDX), new Asm_Registr(AsmEDX)));
		Ans.push_back(new Asm_Unar_Cmd(AsmDiv, new Asm_Registr(AsmEBX)));
		if (Op.Type == TK_DIV_INT) {
			Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Registr(AsmEAX)));
		}
		else {
			Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Registr(AsmEDX)));
		}
		return Ans;
	}
	else {
		for (auto it = BinOperations.cbegin(); it != BinOperations.cend(); ++it) {
			if ((*it).first == Op.Type) {
				Ans.push_back(new Asm_Bin_Cmd((*it).second, new Asm_Registr(AsmEAX), new Asm_Registr(AsmEBX)));
				Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Registr(AsmEAX)));
				return Ans;
			}
		}
	}
}

vector<Asm_Code*> ExprUnarOp::GetAsmCode() {
	vector<Asm_Code*> Ans(Exp->GetAsmCode());
	switch (Op.Type) {
	case TK_PLUS: 
		return Ans;
	case TK_MINUS:	
		Ans.push_back(new Asm_Unar_Cmd(AsmPop, new Asm_Registr(AsmEAX)));
		Ans.push_back(new Asm_Unar_Cmd(AsmNeg, new Asm_Registr(AsmEAX)));
		Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Registr(AsmEAX)));
		return Ans;
	case TK_NOT: 
		Ans.push_back(new Asm_Unar_Cmd(AsmPop, new Asm_Registr(AsmEAX)));
		Ans.push_back(new Asm_Unar_Cmd(AsmNot, new Asm_Registr(AsmEAX)));
		Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Registr(AsmEAX)));
		return Ans;
	}
}

vector<Asm_Code*> ExprIntConst::GetAsmCode() {
	vector<Asm_Code*> Ans;
	Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_IntConst(Value.Source)));
	return Ans;
}

vector<Asm_Code*> ExprFunction::GetAsmCode() {
	vector<Asm_Code*> Ans;
	for (int i = Rights.size() - 1; i >= 0; --i) {
		vector<Asm_Code*> Ret;
		Ret = Rights[i]->GetAsmCode();
		for (int j = 0; j < Ret.size(); ++j) {
			Ans.push_back(Ret[j]);
		}
	}
	if (_stricmp(((ExprVar*)Left)->Sym->Name.c_str(), "write") == 0 || _stricmp(((ExprVar*)Left)->Sym->Name.c_str(), "writeln") == 0) {
		if (Rights.size() != 0) {
			Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Registr(AsmEAX), new Asm_IntConst("\'%d\'")));
			Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Address("fmt", 0), new Asm_Registr(AsmEAX)));
			for (int i = 1; i < Rights.size(); ++i) {
				Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Registr(AsmEAX), new Asm_IntConst("\' %d\'")));
				Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Address("fmt", 2 + (i - 1) * 3), new Asm_Registr(AsmEAX)));
			}
		}

		int offset = 0;
		if (((ExprVar*)Left)->Sym->Name.size() == strlen("writeln")) {
			Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Registr(AsmEAX), new Asm_IntConst("0xA")));
			Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Address("fmt", max(0, (2 + ((int)Rights.size() - 1)*3))), new Asm_Registr(AsmEAX)));
			offset = 1;
		}
		Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Registr(AsmEAX), new Asm_IntConst("0x0")));
		Ans.push_back(new Asm_Bin_Cmd(AsmMov, new Asm_Address("fmt", max(0, (2 + ((int)Rights.size() - 1) * 3 )) + offset), new Asm_Registr(AsmEAX)));

		Ans.push_back(new Asm_Unar_Cmd(AsmPush, new Asm_Variable("fmt"))); 
		Ans.push_back(new Asm_Unar_Cmd(AsmCall, new Asm_Variable("_printf")));
		Ans.push_back(new Asm_Bin_Cmd(AsmAdd, new Asm_Registr(AsmESP), new Asm_IntConst(to_string(4 * Rights.size() + 4))));
	}
	return Ans;
}

