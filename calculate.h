#ifndef CALCULATE_H
#define CALCULATE_H

#include "syntaxnodes.h"

class SymTable;

template <class T> 
class CalculateConstExpr {
private:
	SymTable* Table;
	T CalculateUnarConstExpr(ExprUnarOp* Exp);
	T CalculateBinConstExpr(ExprBinOp* Exp);
	template <class X> void CheckType();
public:
	T Ans;
	T Calculate(Expr* Exp);
	CalculateConstExpr(SymTable* Table);
};

template <class T> CalculateConstExpr<T>::CalculateConstExpr(SymTable* Table) : Table(Table) {}

template <class T> T CalculateConstExpr<T>::Calculate(Expr* Exp) {
	switch (Exp->TypeExp) {
	case VarExp:
		return Calculate(((SymVar*)Table->GetSymbol(((ExprVar*)Exp)->Var.Source.c_str()))->VecExp[0]);
	
	/*case ArrayExp:
		return ((ArrayIndex*)Exp)->Right*/

	case ConstIntExp:
		CheckType<int>();
		return atoi(((ExprIntConst*)Exp)->Value.Source.c_str());

	case ConstRealExp:
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType();
		}
		return atof(((ExprRealConst*)Exp)->Value.Source.c_str());

	case BinExp:
		return CalculateBinConstExpr((ExprBinOp*)Exp);

	case UnarExp:
		return CalculateUnarConstExpr((ExprUnarOp*)Exp);

	default:
		throw BadType();
	}
}

template <class T> T CalculateConstExpr<T>::CalculateUnarConstExpr(ExprUnarOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_MINUS:
		return - Calculate(Exp->Exp);
	case TK_PLUS:
		return + Calculate(Exp->Exp);
	case TK_NOT: 
		return ! Calculate(Exp->Exp);
	default:
		throw BadType();
	}
}

template <class T> T CalculateConstExpr<T>::CalculateBinConstExpr(ExprBinOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_PLUS: 
		return Calculate(Exp->Left) + Calculate(Exp->Right);
	case TK_MINUS: 
		return Calculate(Exp->Left) - Calculate(Exp->Right);
	case TK_MUL: 
		return Calculate(Exp->Left) * Calculate(Exp->Right);
	case TK_DIV_INT:
		CheckType<int>();
	case TK_DIV:
		return Calculate(Exp->Left) / Calculate(Exp->Right);
	case TK_MOD:
		CheckType<int>();
		return (int)Calculate(Exp->Left) % (int)Calculate(Exp->Right);
	case TK_XOR:
		CheckType<int>();
		return (int)Calculate(Exp->Left) ^ (int)Calculate(Exp->Right);
	case TK_AND:
		CheckType<int>();
		return (int)Calculate(Exp->Left) ^ (int)Calculate(Exp->Right);
	case TK_OR:
		CheckType<int>();
		return (int)Calculate(Exp->Left) ^ (int)Calculate(Exp->Right);
	case TK_SHL:
		CheckType<int>();
		return (int)Calculate(Exp->Left) << (int)Calculate(Exp->Right);
	case TK_SHR:
		CheckType<int>();
		return (int)Calculate(Exp->Left) >> (int)Calculate(Exp->Right);
	default:
		throw BadType();
	}
}

template <class T> template <class X> void CalculateConstExpr<T>::CheckType() {
	if (!is_same<X, T>::value) {
		throw BadType();
	}
}

#endif