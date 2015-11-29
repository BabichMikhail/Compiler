#include "checktype.h"
#include "symtable.h"

#define N 8

static MyTypeID CastTable[N][N] = {
	{ TypeID_Integer, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_Double,  TypeID_Double,  TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_Char,    TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Boolean, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_String,  TypeID_BadType, TypeID_String , TypeID_BadType, TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Array,   TypeID_BadType, TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Record,  TypeID_BadType },
	{ TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_BadType, TypeID_Pointer }
};

typedef struct Cell {
	TokenType TT;
	MyTypeID TypeID;
	Cell(TokenType TT, MyTypeID TypeID) : TT(TT), TypeID(TypeID) {};
} Cell;

static vector<Cell> TypeTable[] = {
	{ /* Integer */
		{ TK_PLUS, TypeID_Integer },{ TK_MINUS, TypeID_Integer },{ TK_MUL, TypeID_Integer },{ TK_DIV, TypeID_Double },{ TK_DIV_INT, TypeID_Integer },
		{ TK_MOD, TypeID_Integer },{ TK_EQUAL, TypeID_Boolean },{ TK_NOT_EQUAL, TypeID_Boolean },{ TK_GREAT, TypeID_Boolean },{ TK_LESS, TypeID_Boolean },
		{ TK_GREAT_EQUAL, TypeID_Boolean },{ TK_LESS_EQUAL, TypeID_Boolean },{ TK_XOR, TypeID_Integer },{ TK_SHL, TypeID_Integer },
		{ TK_SHR, TypeID_Integer },{ TK_AND, TypeID_Integer },{ TK_OR, TypeID_Integer }
	},
	{ /* Double */
		{ TK_PLUS, TypeID_Double },{ TK_MINUS, TypeID_Double },{ TK_MUL, TypeID_Double },{ TK_DIV, TypeID_Double },{ TK_EQUAL, TypeID_Boolean },
		{ TK_NOT, TypeID_Boolean },{ TK_GREAT, TypeID_Boolean },{ TK_LESS, TypeID_Boolean },{ TK_GREAT_EQUAL, TypeID_Boolean },
		{ TK_LESS_EQUAL, TypeID_Boolean }
	},
	{ /* Char */ },
	{ /* Boolean */
		{ TK_XOR, TypeID_Boolean },{ TK_AND, TypeID_Boolean },{ TK_OR, TypeID_Boolean }
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

set<int> ReservedProc = { argc_write, argc_writeln };

void CheckType::Check(MyTypeID TypeID_1, MyTypeID TypeID_2) {
	if (!CanCast(TypeID_1, TypeID_2)) {
		throw IncompatibleTypes(StrTypes[TypeID_1 + 1], StrTypes[TypeID_2 + 1], Pos);
	}
}

CheckType::CheckType(SymTable* Table, const Position Pos) : Table(Table), Pos(Pos) {}

CheckType::CheckType(SymTable* Table, Expr* Exp_Left, Expr* Exp_Right, const Position Pos) : Table(Table), Pos(Pos) {
	Check(GetTypeID(Exp_Left), GetTypeID(Exp_Right));
}

CheckType::CheckType(SymTable* Table, Expr* ExpAssign, const Position Pos) : Table(Table), Pos(Pos) {
	if (ExpAssign->TypeExp != AssignExp) {
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
			CheckType(Table, Sym->GetType(), ((ExprInitList*)Exp)->List[i], Pos);
		}
		return;
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
		//auto ASym = (SymConst*)Sym;
		if (Sym->GetType() != nullptr) {
			return ((SymType*)Sym->GetType())->TypeID;
		}
		auto TypeID = GetTypeID(((SymConst*)Sym)->InitExp->TypeExp);
		if (TypeID != TypeID_String) {
			return TypeID;
		}
		if (TK.Source.size() == 1) {
			return TypeID_Char;
		}
		else {
			int Length = ((SymStringType*)Sym->GetType())->Length;
			if (TK.Source.size() > Length && Length != -1) {
				throw IncompatibleTypes(StrTypes[TypeID_String + 1], StrTypes[TypeID + 1], Pos);
			}
			return TypeID_String;
		}
	}
	if (Sym->Section == DeclVar) {
		auto TypeID = ((SymType*)Sym->GetType())->TypeID;
		if (TypeID != TypeID_String) {
			return TypeID;
		}
		if (TK.Source.size() == 1) {
			return TypeID_Char;
		}
		else {
			int Length = ((SymStringType*)Sym->GetType())->Length;
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
		Expr* AIExp = Exp;
		int Count = 0;
		while (AIExp->TypeExp != VarExp && AIExp->TypeExp != RecordExp) {
			if (AIExp->TypeExp == ArrayExp) {
				AIExp = ((ExprArrayIndex*)AIExp)->Left;
				++Count;
			}
			else if (AIExp->TypeExp == DereferenceExp) {
				AIExp = ((ExprDereference*)AIExp)->Exp;
			}
			else if (AIExp->TypeExp == FunctionExp) {
				AIExp = ((ExprFunction*)AIExp)->Left;
			}
		}
		Symbol* _Sym = nullptr;
		if (AIExp->TypeExp == VarExp) {
			_Sym = ((ExprIdent*)AIExp)->Sym;
		}
		else if (AIExp->TypeExp == RecordExp) {
			_Sym = ((ExprRecord*)AIExp)->Right;
		}
		for (int i = Count; i >= 0; --i) {
			if (_Sym->Section == DeclType) {
				_Sym = _Sym->GetType();
			}
			else if (_Sym->Section == DeclVar || _Sym->Section == DeclConst){
				_Sym = _Sym->GetType();
			}
			else if (_Sym->Section == DeclFunction) {
				_Sym = _Sym->GetType();
			}
		}
		return ((SymType*)_Sym)->TypeID;
	}
	if (Exp->TypeExp == UnarExp) {
		return GetTypeID(((ExprUnarOp*)Exp)->Exp);
	}
	if (Exp->TypeExp == DereferenceExp) {
		return GetTypeID(((ExprDereference*)Exp)->Exp);
	}
	if (Exp->TypeExp == VarExp) {
		if ((((ExprIdent*)Exp)->Sym)->GetType()->Name == "pointer") {
			return ((SymType*)(((ExprIdent*)Exp)->Sym)->GetType()->GetType())->TypeID;
		}
		if (((ExprIdent*)Exp)->Sym->GetType()->Section == DeclRecord) {
			return TypeID_Record;
		}
		return ((SymType*)(((ExprIdent*)Exp)->Sym)->GetType())->TypeID;
	}
	if (Exp->TypeExp == ConstStringExp) {
		if (((ExprStringConst*)Exp)->Value.Source.size() == 1) {
			return TypeID_Char;
		}
	}
	if (Exp->TypeExp == RecordExp) {
		auto TypeID = ((SymType*)((ExprRecord*)Exp)->Right->GetType())->TypeID;
		return TypeID;
	}
	if (Exp->TypeExp == FunctionExp) {
		auto Symbol = Table->FindRequiredSymbol(Exp, Pos);
		if (Symbol->Section == DeclProcedure) {
			return TypeID_BadType;
		}
		else {
			if (Symbol->GetType()->Section == DeclRecord) {
				return TypeID_Record;
			}
			return ((SymType*)Symbol->GetType())->TypeID;
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
	else {
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
		return Compare(Type_1->GetType(), Type_2->GetType());
	case TypeID_Record:
		Ans = true;
		if (((SymRecord*)Type_1->GetType())->argc != ((SymRecord*)(Type_2->GetType()))->argc) {
			return false;
		}
		for (int i = 0; i < ((SymRecord*)Type_1->GetType())->Table->Symbols.size(); ++i) {
			Ans = Ans && Compare(((SymRecord*)Type_1->GetType())->Table->Symbols[i], ((SymRecord*)Type_2->GetType())->Table->Symbols[i]);
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
		if (Sym_R_1->Table->Symbols[i]->GetType() != Sym_R_2->Table->Symbols[i]->GetType()) {
			return false;
		}
	}
	return true;
}
