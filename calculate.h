#ifndef CALCULATE_H
#define CALCULATE_H

#include "syntaxnodes.h"

class SymTable;

template <class T> 
class CalculateConstExpr {
private:
	SymTable* Table;
	string TypeName;
	T CalculateUnarConstExpr(ExprUnarOp* Exp);
	T CalculateBinConstExpr(ExprBinOp* Exp);
	T CalculateArrayIndex(ArrayIndex* Exp);
	template <class X> void CheckType();
public:
	T Ans;
	T Calculate(Expr* Exp);
	CalculateConstExpr(SymTable* Table, string TypeName);
};

template <class T> CalculateConstExpr<T>::CalculateConstExpr(SymTable* Table, string TypeName) : Table(Table), TypeName(TypeName) {}

template <class T> T CalculateConstExpr<T>::Calculate(Expr* Exp) {
	switch (Exp->TypeExp) {
	case VarExp:
		return Calculate(((SymVar*)Table->GetSymbol(((ExprVar*)Exp)->Var.Source.c_str()))->InitExp);
	
	case ConstIntExp:
		CheckType<int>();
		return atoi(((ExprIntConst*)Exp)->Value.Source.c_str());

	case ConstRealExp:
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
		return atof(((ExprRealConst*)Exp)->Value.Source.c_str());

	case ConstBoolExp:
		CheckType<bool>();
		return _strnicmp(((ExprBoolConst*)Exp)->Value.Source.c_str(), "true", 4) == 0 ? true : false;

	case ConstStringExp:
		throw BadType("DOUBLE\" || \"INTEGER\" || \"BOOL");

	case BinExp:
		return CalculateBinConstExpr((ExprBinOp*)Exp);

	case UnarExp:
		return CalculateUnarConstExpr((ExprUnarOp*)Exp);

	case ArrayExp:
		return CalculateArrayIndex((ArrayIndex*)Exp);
	}
}

template <class T> T CalculateConstExpr<T>::CalculateUnarConstExpr(ExprUnarOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_MINUS:
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
		return - Calculate(Exp->Exp);
	case TK_PLUS:
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
		return + Calculate(Exp->Exp);
	case TK_NOT: 
		return ! Calculate(Exp->Exp);
	}
}

template <class T> T CalculateConstExpr<T>::CalculateBinConstExpr(ExprBinOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_PLUS: 
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
		return Calculate(Exp->Left) + Calculate(Exp->Right);
	case TK_MINUS: 
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
		return Calculate(Exp->Left) - Calculate(Exp->Right);
	case TK_MUL: 
		if (!is_same<T, int>::value && !is_same<T, double>::value) {
			throw BadType(TypeName);
		}
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
	}
}

template <class T> T CalculateConstExpr<T>::CalculateArrayIndex(ArrayIndex* Exp) {
	Expr* IdentExp = Exp;
	vector<int> idxs;
	while (IdentExp->TypeExp != VarExp) {
		idxs.push_back(Calculate(((ArrayIndex*)IdentExp)->Right));
		IdentExp = ((ArrayIndex*)IdentExp)->Left;
	}
	InitList* ExpList = (InitList*)((SymConst*)Table->GetSymbol(((ExprVar*)IdentExp)->Var.Source.c_str()))->InitExp;
	IdentExp = Exp;
	for (int i = idxs.size() - 1; i >= 0; --i) {
		ExpList = (InitList*)ExpList->List[idxs[i]];
	}
	return Calculate(ExpList);
}

template <class T> template <class X> void CalculateConstExpr<T>::CheckType() {
	if (!is_same<X, T>::value) {
		throw BadType(TypeName);
	}
}

#endif