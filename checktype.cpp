#include "checktype.h"

#define N 9

static MyTypeID CastTable[N][N] = {
	{ TypeID_Integer, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_Double,  TypeID_Double,  TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_Char,    TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Boolean, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_String,  TypeID_BadType, TypeID_String , TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Array,   TypeID_BadType,  TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_DynArray, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_Record,  TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType,  TypeID_BadType, TypeID_Pointer }
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
	{ /* Array */ },
	{ /* Record */ },
	{ /* Pointer */
		{ TK_MINUS, TypeID_Integer }
	}
};

const string StrTypes[] = {
	"Untyped", "Integer", "Double", "Char", "Boolean", "String", "Array", "Record", "Pointer", "Function"
};

/* TypeID_1 - тип, к которому приводится TypeID_2 */

void CheckType::Check(MyTypeID TypeID_1, MyTypeID TypeID_2) {
	if (!CanCast(TypeID_1, TypeID_2)) {
		throw IncompatibleTypes(StrTypes[TypeID_1 + 1], StrTypes[TypeID_2 + 1], Pos);
	}
}

CheckType::CheckType(SymTable* Table, const Position Pos): Table(Table), Pos(Pos){}

CheckType::CheckType(SymTable* Table, Expr* Exp_Left, Expr* Exp_Right, const Position Pos): Table(Table), Pos(Pos) {
	Check(GetTypeID(Exp_Left), GetTypeID(Exp_Right));
}

CheckType::CheckType(SymTable* Table, Expr* ExpAssign, const Position Pos) : Table(Table), Pos(Pos){
	if (ExpAssign->TypeExp != AssignExp){ 
		throw IllegalExpr(Pos);
	}
	Check(GetTypeID(((ExprAssign*)ExpAssign)->Left), GetTypeID(((ExprAssign*)ExpAssign)->Right));
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
	case TypeID_Record:
		throw IncompatibleTypes(StrTypes[TypeID_Record + 1], StrTypes[GetTypeID(Exp->TypeExp) + 1], Pos);
	case TypeID_Array:
		if (Exp->TypeExp != InitExp || ((ExprInitList*)Exp)->List.size() != ((SymArray*)Sym)->Right - ((SymArray*)Sym)->Left + 1) {
			throw IncompatibleTypes(StrTypes[TypeID_Array + 1], StrTypes[GetTypeID(Exp->TypeExp) + 1], Pos);
		}
		for (int i = 0; i < ((ExprInitList*)Exp)->List.size(); ++i) {
			CheckType(Table, ((SymArray*)Sym)->Type, ((ExprInitList*)Exp)->List[i], Pos);
		}
		return;
	case TypeID_DynArray:
		throw IncompatibleTypes(StrTypes[TypeID_DynArray + 1], StrTypes[GetTypeID(Exp->TypeExp) + 1], Pos);
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
	case PointerExp:
		return TypeID_Pointer;
	}
	throw IllegalExpr(Pos);
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
				throw IncompatibleTypes(StrTypes[TypeID_String + 1], StrTypes[TypeID + 1], Pos);
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
				throw IncompatibleTypes(StrTypes[TypeID_String + 1], StrTypes[TypeID_BadType + 1], Pos);
			}
			return TypeID_String;
		}
	}
}

MyTypeID CheckType::GetTypeID(Expr* Exp) {
	if (Exp->TypeExp == BinExp) {
		return GetTypeID_BinExp(GetTypeID(((ExprBinOp*)Exp)->Left), GetTypeID(((ExprBinOp*)Exp)->Right), ((ExprBinOp*)Exp)->Op.Type);
	}
	if (Exp->TypeExp == ArrayExp) {
		auto AIExp = Exp;
		while (((ExprArrayIndex*)AIExp)->Left->TypeExp != VarExp) {
			AIExp = ((ExprArrayIndex*)AIExp)->Left;
		}
		auto _Sym = ((SymVar*)((ExprVar*)((ExprArrayIndex*)AIExp)->Left)->Sym)->Type;
		AIExp = Exp;
		while (((ExprArrayIndex*)AIExp)->TypeExp != VarExp) {
			AIExp = ((ExprArrayIndex*)AIExp)->Left;
			_Sym = ((SymArray*)_Sym)->Type;
		}
		return ((SymType*)_Sym)->TypeID;
	}
	if (Exp->TypeExp == UnarExp) {
		return GetTypeID(((ExprUnarOp*)Exp)->Exp);
	}
	if (Exp->TypeExp == DereferenceExp) {
		Check(TypeID_Pointer, GetTypeID(((ExprDereference*)Exp)->Exp));
	}
	if (Exp->TypeExp == VarExp) {
		return ((SymType*)((SymVar*)(((ExprVar*)Exp)->Sym))->Type)->TypeID;
	}
	if (Exp->TypeExp == ConstStringExp) {
		if (((ExprStringConst*)Exp)->Value.Source.size() == 1) {
			return TypeID_Char;
		}
	}
	if (Exp->TypeExp == RecordExp) {
		auto TypeID = ((SymType*)((SymVar*)((ExprRecord*)Exp)->Right)->Type)->TypeID;
		return TypeID;
	}
	if (Exp->TypeExp == FunctionExp) {
		auto Symbols = Table->GetAllSymbols(((ExprVar*)((ExprFunction*)Exp)->Left)->Sym->Name, Pos);
		for (int i = 0; i < Symbols.size(); ++i) {
			if (Symbols[i]->Section == DeclFunction) {
				if (((ExprFunction*)Exp)->Rights.size() != ((SymFunction*)Symbols[i])->argc - 1) {
					continue;
				}
			}
			if (Symbols[i]->Section == DeclProcedure) {
				if (((ExprFunction*)Exp)->Rights.size() != ((SymFunction*)Symbols[i])->argc) {
					continue;
				}
			}
			bool flag = false;
			if (Symbols[i]->Section == DeclFunction || Symbols[i]->Section == DeclProcedure) {
				int offset = ((SymFunction*)Symbols[i])->Table->DeclTypeCount;
				for (int j = 0; j < ((ExprFunction*)Exp)->Rights.size(); ++j) {
					if (GetTypeID(((ExprFunction*)Exp)->Rights[j]) != ((SymType*)((SymVar*)((SymCall*)Symbols[i])->Table->Symbols[offset + j])->Type)->TypeID) {
						flag = true;
					}
				}
			}
			if (flag) {
				continue;
			}
			if (Symbols[i]->Section == DeclFunction) {
				return ((SymType*)((SymFunction*)Symbols[i])->Type)->TypeID;
			}
			if (Symbols[i]->Section == DeclProcedure) {
				return TypeID_BadType;
			}
		}
		throw IllegalExpr(Pos);
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
		throw IncompatibleTypes(StrTypes[TypeID_Left + 1], StrTypes[TypeID_Right + 1], Pos);
	}
	for (int i = 0; i < TypeTable[TypeID_Left].size(); ++i) {
		if (TypeTable[TypeID_Left][i].TT == OpType) {
			return TypeTable[TypeID_Left][i].TypeID;
		}
	}
	return TypeID_BadType;
}

bool CheckType::CanCast(MyTypeID TypeID_1, MyTypeID TypeID_2) {
	return CastTable[TypeID_1][TypeID_2] != TypeID_BadType;
}

/* CmpArguments */

bool CmpArguments::CompareTypes(Symbol* Type_1, Symbol* Type_2) {
	if (((SymType*)Type_1)->TypeID != ((SymType*)Type_2)->TypeID) {
		return false;
	}
	bool Ans;
	switch (((SymType*)Type_1)->TypeID) {
	case TypeID_Boolean:
	case TypeID_Char:
	case TypeID_Double:
	case TypeID_Integer:
		return true;
	case TypeID_Array:
	case TypeID_DynArray:
		return Compare(((SymType*)Type_1)->Type, ((SymType*)Type_2)->Type);
	case TypeID_Record:
		Ans = true;
		if (((SymRecord*)((SymType*)Type_1)->Type)->argc != ((SymRecord*)((SymType*)Type_2)->Type)->argc) {
			return false;
		}
		for (int i = 0; i < ((SymRecord*)((SymType*)Type_1)->Type)->Table->Symbols.size(); ++i) {
			Ans = Ans && Compare(((SymRecord*)((SymType*)Type_1)->Type)->Table->Symbols[i], ((SymRecord*)((SymType*)Type_2)->Type)->Table->Symbols[i]);
		}
		return Ans;
	default:
		return false;
	}
}

bool CmpArguments::Compare(Symbol* Sym_1, Symbol* Sym_2) {
	auto Sym_R_1 = (SymFunction*)Sym_1;
	auto Sym_R_2 = (SymFunction*)Sym_2;
	if (Sym_R_1->argc != Sym_R_2->argc) {
		return false;
	}
	for (int i = 0; i < Sym_R_1->argc; ++i) {
		if (((SymVar*)Sym_R_1->Table->Symbols[i])->Type != ((SymVar*)Sym_R_2->Table->Symbols[i])->Type) {
			return false;
		}
	}
	return true;
}