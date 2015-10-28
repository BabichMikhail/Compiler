#include "syntaxnodes.h"
#include <vector>
#include <set>

#define indent "   "

Expr::Expr(TypeExpr TypeExp) : TypeExp(TypeExp){}

ExprBinOp::ExprBinOp(Expr* Left, Token Op, Expr* Right) : Left(Left), Op(Op), Right(Right), Expr(BinExp){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp), Expr(UnarExp){}

ExprConst::ExprConst(Token Value, TypeExpr TypeExp) : Value(Value), Expr(TypeExp){}
ExprBoolConst::ExprBoolConst(Token Value) : ExprConst(Value, ConstBoolExp){}
ExprIntConst::ExprIntConst(Token Value) : ExprConst(Value, ConstIntExp){}
ExprRealConst::ExprRealConst(Token Value) : ExprConst(Value, ConstRealExp){}
ExprStringConst::ExprStringConst(Token Value) : ExprConst(Value, ConstStringExp){}

ExprVar::ExprVar(Token Var) : Var(Var), Expr(VarExp){}

ArrayIndex::ArrayIndex(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(ArrayExp){}
Assign::Assign(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(AssignExp){}
Function::Function(Expr* Left, vector<Expr*> Rights) : Left(Left), Rights(Rights), Expr(FunctionExp){}
Record::Record(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(RecordExp){}

void ExprUnarOp::Print(int Spaces){
	Exp->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Op.Source.c_str() << endl;
}

void ExprBinOp::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Op.Source.c_str() << endl;
	Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Value.Source.c_str() << endl;
}

void ExprVar::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Var.Source.c_str() << endl;
}

void Assign::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << ":=" << endl;
	Left->Print(Spaces + 1);
}

void Function::Print(int Spaces){
	for (int i = 0; i < Rights.size(); ++i){
		Rights[i]->Print(Spaces + 1);
	}
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "()" << endl;
	Left->Print(Spaces + 1);
}

void Record::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "." << endl;
	Left->Print(Spaces + 1);
}

void ArrayIndex::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "[]" << endl;
	Left->Print(Spaces + 1);
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
	List->Vec.push_back(Var.Source);
}

void Assign::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void ArrayIndex::GetIdentStr(ExpArgList* List){
	Left->GetIdentStr(List);
	Right->GetIdentStr(List);
}

void Function::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void Record::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}