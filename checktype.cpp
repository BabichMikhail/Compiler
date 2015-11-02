#include "checktype.h"

static MyTypeID CastTable[7][7] = {
	{ TypeID_Integer, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_Double,  TypeID_Double,  TypeID_Double,  TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_Char,    TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Boolean, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_String,  TypeID_BadType, TypeID_String , TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Array,   TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_DynArray }
};

typedef struct Cell {
	TokenType TT;
	MyTypeID TypeID;
	Cell(TokenType TT, MyTypeID TypeID) : TT(TT), TypeID(TypeID) {};
} Cell;

static vector<Cell> TypeTable[] = {
	{ /* Integer */
		{TK_PLUS, TypeID_Integer}, {TK_MINUS, TypeID_Integer}, {TK_MUL, TypeID_Integer}, {TK_DIV, TypeID_Double}, {TK_DIV_INT, TypeID_Integer},
		{TK_MOD, TypeID_Integer}, {TK_EQUAL, TypeID_Boolean}, {TK_NOT_EQUAL, TypeID_Boolean}, {TK_GREAT, TypeID_Boolean}, {TK_LESS, TypeID_Boolean},
		{TK_GREAT_EQUAL, TypeID_Boolean}, {TK_LESS_EQUAL, TypeID_Boolean}, {TK_XOR, TypeID_Integer}, {TK_SHL, TypeID_Integer}, 
		{TK_SHR, TypeID_Integer}, {TK_AND, TypeID_Integer}, {TK_OR, TypeID_Integer}
	},
	{ /* Double */
		{ TK_PLUS, TypeID_Double }, { TK_MINUS, TypeID_Double }, { TK_MUL, TypeID_Double }, { TK_DIV, TypeID_Double }, { TK_EQUAL, TypeID_Boolean },
		{ TK_NOT, TypeID_Boolean }, { TK_GREAT, TypeID_Boolean }, { TK_LESS, TypeID_Boolean }, { TK_GREAT_EQUAL, TypeID_Boolean },
		{ TK_LESS_EQUAL, TypeID_Boolean }
	},
	{ /* Char */ },
	{ /* Boolean */
		{TK_XOR, TypeID_Boolean}, {TK_AND, TypeID_Boolean}, {TK_OR, TypeID_Boolean}
	},
	{ /* String */ },
	{ /* Array */ }
};

/* TypeID_1 - тип, к которому приводится TypeID_2 */

void CheckType::Check(MyTypeID TypeID_1, MyTypeID TypeID_2) {
	if (!CanCast(TypeID_1, TypeID_2)) {
		throw IncompatibleTypes(Pos);
	}
}

CheckType::CheckType(SymTable* Table, Expr* Exp_Left, Expr* Exp_Right, const Position Pos): Table(Table), Pos(Pos) {
	Check(GetTypeID(Exp_Left), GetTypeID(Exp_Right));
}

CheckType::CheckType(SymTable* Table, Expr* ExpAssign, const Position Pos) : Table(Table), Pos(Pos){
	if (ExpAssign->TypeExp != AssignExp){ 
		throw IllegalExpr(Pos);
	}
	Check(GetTypeID(((Assign*)ExpAssign)->Left), GetTypeID(((Assign*)ExpAssign)->Right));
}

CheckType::CheckType(SymTable* Table, Symbol* Sym, Expr* Exp, const Position Pos) : Table(Table), Pos(Pos) {
	switch (((SymType*)Sym)->TypeID) {
	case TypeID_Integer:
	case TypeID_Double:
	case TypeID_Char:
	case TypeID_Boolean:
	case TypeID_String:
		Check(((SymType*)Sym)->TypeID, GetTypeID(Exp));
		return;
	case TypeID_Array:
		if (Exp->TypeExp != InitExp) {
			throw IncompatibleTypes(Pos);
		}
		if (((InitList*)Exp)->List.size() != ((SymArray*)Sym)->Right - ((SymArray*)Sym)->Left + 1) {
			throw IncompatibleTypes(Pos);
		}
		for (int i = 0; i < ((InitList*)Exp)->List.size(); ++i) {
			CheckType(Table, ((SymArray*)Sym)->Type, ((InitList*)Exp)->List[i], Pos);
		}
		return;
	case TypeID_DynArray:
		throw IncompatibleTypes(Pos);
	}
}

CheckType::CheckType(SymTable* Table, MyTypeID TypeID_1, Expr* Exp, const Position Pos) : Table(Table), Pos(Pos) {
	Check(TypeID_1, GetTypeID(Exp));
}

MyTypeID CheckType::GetTypeID(TypeExpr TypeExp) {
	switch (TypeExp) {
	case ArrayExp:
		return TypeID_Array;
	case ConstBoolExp:
		return TypeID_Boolean;
	case ConstDoubleExp:
		return TypeID_Double;
	case ConstIntExp:
		return TypeID_Integer;
	case ConstStringExp:
		return TypeID_String;
	}
}

MyTypeID CheckType::GetTypeID(Token TK) {
	auto Sym = Table->GetSymbol(TK.Source, TK.Pos);
	if (Sym->Section != DeclConst && Sym->Section != DeclVar) {
		throw;
	}
	if (Sym->Section == DeclConst) {
		auto ASym = (SymConst*)Sym;
		if (ASym->Type != nullptr) {
			return ((SymType*)ASym->Type)->TypeID;
		}
		auto TypeID = GetTypeID(ASym->InitExp->TypeExp);
		if (TypeID != TypeID_String) {
			return TypeID;
		}
		if (TK.Source.size() == 1) {
			return TypeID_Char;
		}
		else {
			int Length = ((SymStringType*)((SymType*)ASym)->Type)->Length;
			if (TK.Source.size() > Length && Length != -1) {
				throw IncompatibleTypes(Pos);
			}
			return TypeID_String;
		}
	}
	if (Sym->Section == DeclVar) {
		auto ASym = (SymVar*)Sym;
		auto TypeID = ((SymType*)ASym->Type)->TypeID;
		if (TypeID != TypeID_String) {
			return TypeID;
		}
		if (TK.Source.size() == 1) {
			return TypeID_Char;
		}
		else {
			int Length = ((SymStringType*)((SymType*)ASym)->Type)->Length;
			if (TK.Source.size() > Length && Length != -1) {
				throw IncompatibleTypes(Pos);
			}
			return TypeID_String;
		}
	}
	throw IncompatibleTypes(Pos);
}

MyTypeID CheckType::GetTypeID(Expr* Exp) {
	if (Exp->TypeExp == BinExp) {
		return GetTypeID_BinExp(GetTypeID(((ExprBinOp*)Exp)->Left), GetTypeID(((ExprBinOp*)Exp)->Right), ((ExprBinOp*)Exp)->Op.Type);
	}
	if (Exp->TypeExp == UnarExp) {
		return GetTypeID(((ExprUnarOp*)Exp)->Exp);
	}
	if (Exp->TypeExp == VarExp) {
		return GetTypeID(((ExprVar*)Exp)->Var);
	}
	if (Exp->TypeExp == ConstStringExp) {
		if (((ExprStringConst*)Exp)->Value.Source.size() == 1) {
			return TypeID_Char;
		}
	}
	return GetTypeID(Exp->TypeExp);
}

MyTypeID CheckType::GetTypeID_BinExp(MyTypeID TypeID_Left, MyTypeID TypeID_Right, TokenType OpType) {
	if (CanCast(TypeID_Left, TypeID_Right)) {
		TypeID_Right = TypeID_Left;
	}
	else if (CanCast(TypeID_Right, TypeID_Left)) {
		TypeID_Left = TypeID_Right;
	}
	else  {
		throw IncompatibleTypes(Pos);
	}
	for (int i = 0; i < TypeTable[TypeID_Left].size(); ++i) {
		if (TypeTable[TypeID_Left][i].TT == OpType) {
			return TypeTable[TypeID_Left][i].TypeID;
		}
	}
}

bool CheckType::CanCast(MyTypeID TypeID_1, MyTypeID TypeID_2) {
	return CastTable[TypeID_1][TypeID_2] != TypeID_BadType;
}