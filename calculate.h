#ifndef CALCULATE_H
#define CALCULATE_H

#include "syntaxnodes.h"

class SymTable;

template <class T> 
class CalculateConstExpr {
private:
	Position Pos;
	SymTable* Table;
	string TypeName;
	T CalculateUnarConstExpr(ExprUnarOp* Exp);
	T CalculateBinConstExpr(ExprBinOp* Exp);
	T CalculateArrayIndex(ExprArrayIndex* Exp);
	template <class X_1, class X_2 = T> void CheckType();
public:
	T Ans;
	T Calculate(Expr* Exp);
	CalculateConstExpr(SymTable* Table, string TypeName, Position Pos);
};

template <class T> CalculateConstExpr<T>::CalculateConstExpr(SymTable* Table, string TypeName, Position Pos) : Table(Table), TypeName(TypeName), Pos(Pos){}

template <class T> T CalculateConstExpr<T>::Calculate(Expr* Exp) {
	switch (Exp->TypeExp) {
	case VarExp:
		return Calculate(((SymVar*)Table->GetSymbol(((ExprIdent*)Exp)->Sym->Name.c_str(), Pos))->InitExp);
	
	case ConstIntExp:
		CheckType<int>();
		return atoi(((ExprIntConst*)Exp)->Value.Source.c_str());

	case ConstDoubleExp:
		CheckType<int, double>();
		return atof(((ExprDoubleConst*)Exp)->Value.Source.c_str());

	case ConstBoolExp:
		CheckType<bool>();
		return _strnicmp(((ExprBoolConst*)Exp)->Value.Source.c_str(), "true", 4) == 0 ? true : false;

	case ConstStringExp:
		if (((ExprStringConst*)Exp)->Value.Source.size() > 1) {
			throw BadType("DOUBLE\" || \"INTEGER\" || \"BOOL", Pos);
		}
		else {
			return atoi(((ExprStringConst*)Exp)->Value.Source.c_str());
		}

	case BinExp:
		return CalculateBinConstExpr((ExprBinOp*)Exp);

	case UnarExp:
		return CalculateUnarConstExpr((ExprUnarOp*)Exp);

	case ArrayExp:
		return CalculateArrayIndex((ExprArrayIndex*)Exp);
	}
}

template <class T> T CalculateConstExpr<T>::CalculateUnarConstExpr(ExprUnarOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_MINUS:
		CheckType<int, double>();
		return - Calculate(Exp->Exp);
	case TK_PLUS:
		CheckType<int, double>();
		return + Calculate(Exp->Exp);
	case TK_NOT: 
		return ! Calculate(Exp->Exp);
	}
}

template <class T> T CalculateConstExpr<T>::CalculateBinConstExpr(ExprBinOp* Exp) {
	switch (Exp->Op.Type) {
	case TK_PLUS: 
		CheckType<int, double>();
		return Calculate(Exp->Left) + Calculate(Exp->Right);
	case TK_MINUS: 
		CheckType<int, double>();
		return Calculate(Exp->Left) - Calculate(Exp->Right);
	case TK_MUL: 
		CheckType<int, double>();
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
		return (int)Calculate(Exp->Left) & (int)Calculate(Exp->Right);
	case TK_OR:
		CheckType<int>();
		return (int)Calculate(Exp->Left) | (int)Calculate(Exp->Right);
	case TK_SHL:
		CheckType<int>();
		return (int)Calculate(Exp->Left) << (int)Calculate(Exp->Right);
	case TK_SHR:
		CheckType<int>();
		return (int)Calculate(Exp->Left) >> (int)Calculate(Exp->Right);
	}
}

template <class T> T CalculateConstExpr<T>::CalculateArrayIndex(ExprArrayIndex* Exp) {
	Expr* IdentExp = Exp;
	vector<int> idxs;
	while (IdentExp->TypeExp != VarExp) {
		idxs.push_back(Calculate(((ExprArrayIndex*)IdentExp)->Right));
		IdentExp = ((ExprArrayIndex*)IdentExp)->Left;
	}
	ExprInitList* ExpList = (ExprInitList*)((SymConst*)Table->GetSymbol(((ExprIdent*)IdentExp)->Sym->Name.c_str(), Pos))->InitExp;
	IdentExp = Exp;
	for (int i = idxs.size() - 1; i >= 0; --i) {
		ExpList = (ExprInitList*)ExpList->List[idxs[i]];
	}
	return Calculate(ExpList);
}

template <class T> template <class X_1, class X_2 = T> void CalculateConstExpr<T>::CheckType() {
	if (!is_same<X_1, T>::value && !is_same<X_2, T>::value) {
		throw BadType(TypeName, Pos);
	}
}

#endif