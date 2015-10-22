#include "parser.h"
#include "errors.h"

using namespace std;

#define parse_factor_level 3

set<TokenType> sym_types = { TK_INTEGER, TK_REAL, TK_CHAR, TK_BOOLEAN, TK_STRING, TK_ARRAY };

set<TokenType> start_expr_tk = { TK_INTEGER_VALUE, TK_REAL_VALUE, TK_OPEN_BRACKET, TK_IDENTIFIER, TK_PLUS, TK_MINUS, TK_TRUE, TK_FALSE, TK_NOT, TK_STRING_VALUE };

set<TokenType> rel_op = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };
set<TokenType> add_op = { TK_PLUS, TK_MINUS, TK_OR, TK_XOR };
set<TokenType> mul_op = { TK_MUL, TK_DIV, TK_DIV_INT, TK_MOD, TK_AND, TK_SHL, TK_SHR };
set<TokenType> simp_stat = { TK_OPEN_SQUARE_BRACKET, TK_OPEN_BRACKET, TK_POINT };
vector<set<TokenType>> level_list = { rel_op, add_op, mul_op };

set<TypeExpr> Left_Op_Assign = { VarExp, RecordExp, ArrayExp };

void Parser::Print(){
	if (Exp != nullptr)
		Exp->Print(0);
	Table.Print();
}

Parser::Parser(const char* filename, PState State) : Lex(filename){
	switch (State){
	case Test_Exp:
		while (Lex.isToken()){
			Lex.Next();
			if (start_expr_tk.find(Lex.Get().Type) != start_expr_tk.end()){
				Exp = ParseExpr();
				//continue;
			}
		}
		break;
	case Test_Def:
		Lex.Next();
		while (Lex.isToken()){
			switch (Lex.Get().Type){
			case TK_LABEL: 
				ParseLabel();
				break;
			case TK_CONST:
				ParseConst();
				break;
			case TK_TYPE:
				ParseType();
				break;
			//case TK_FUNCTION:
			//	ParseFunction();
			//	break;
			//case TK_PROCEDURE:
			//	ParseProcedure();
			//	break;
			case TK_VAR:
				ParseVar();
				break; 
			default:
				throw UnexpectedSymbol("BEGIN", Lex.Get().Source);
			}
		}
		break;
	}
}

/* Parse Declarations */

void Parser::ParseLabel(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Sym = new SymLabel(State_Label, Lex.Get().Source);
		Lex.Next();
		Lex.AssertAndNext(TK_SEMICOLON);
		Table.Add(Sym);
	}
}

void Parser::ParseConst(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Name = Lex.Get().Source;
		Lex.Next(); 
		if (Lex.Get().Type == TK_COLON){
			Lex.Next();
			if (sym_types.find(Lex.Get().Type) == sym_types.cend() && Table.Find(Lex.Get().Source) == -1){
				throw UnknownType(Lex.Get().Source);
			}
			Symbol* Sym;
			switch (Lex.Get().Type){
			case TK_INTEGER:
				Table.Add(ParseSimpleType<SymIntType>(State_Const, Name));
				break;
			case TK_REAL:
				Table.Add(ParseSimpleType<SymRealType>(State_Const, Name));
				break;
			case TK_CHAR:
				Table.Add(ParseSimpleType<SymCharType>(State_Const, Name));
				break;
			case TK_BOOLEAN:
				Table.Add(ParseSimpleType<SymBoolType>(State_Const, Name));
				break;
			case TK_STRING:
				Table.Add(ParseString(State_Const, Name));
				break;
			case TK_ARRAY:
				Table.Add(ParseArray(State_Const, Name));
				break;
			case TK_IDENTIFIER:
				Table.Add(ParseIdentifier(State_Const, Name, Lex.Get().Source));
				break;
			}
			Lex.AssertAndNext(TK_SEMICOLON);
			continue;
		}
		Lex.Assert(TK_EQUAL);
		Table.Add(new SymTypeID(State_Const, Name, ParseEqual(), ""));
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

//void Parser::ParseFunction(){}
//void Parser::ParseProcedure(){}

void Parser::ParseVar(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Name = Lex.Get().Source;
		Lex.NextAndAssert(TK_COLON);
		Lex.Next();
		if (sym_types.find(Lex.Get().Type) == sym_types.cend() && Table.Find(Lex.Get().Source, State_Type) == -1){
			throw UnknownType(Lex.Get().Source);
		}
		switch (Lex.Get().Type){
		case TK_INTEGER: 
			Table.Add(ParseSimpleType<SymIntType>(State_Var, Name));
			break;
		case TK_REAL: 
			Table.Add(ParseSimpleType<SymRealType>(State_Var, Name));
			break;
		case TK_CHAR: 
			Table.Add(ParseSimpleType<SymCharType>(State_Var, Name));
			break;
		case TK_BOOLEAN:
			Table.Add(ParseSimpleType<SymBoolType>(State_Var, Name));
			break;
		case TK_STRING:
			Table.Add(ParseString(State_Var, Name));
			break;
		case TK_ARRAY:
			Table.Add(ParseArray(State_Var, Name));
			break;
		case TK_IDENTIFIER: 
			Table.Add(ParseIdentifier(State_Var, Name, Lex.Get().Source));
			break;
		}
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseType(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		string NameNew = Lex.Get().Source;
		Lex.NextAndAssert(TK_EQUAL);
		Lex.Next();
		if (sym_types.find(Lex.Get().Type) == sym_types.cend() && Table.Find(Lex.Get().Source) == -1){
			throw UnknownType(Lex.Get().Source);
		}
		Symbol* Sym = nullptr;
		switch (Lex.Get().Type){
		case TK_INTEGER: 
			Sym = ParseSimpleType<SymIntType>(State_Type); 
			break;
		case TK_REAL: 
			Sym = ParseSimpleType<SymRealType>(State_Type); 
			break;
		case TK_CHAR: 
			Sym = ParseSimpleType<SymCharType>(State_Type); 
			break;
		case TK_BOOLEAN: 
			Sym = ParseSimpleType<SymBoolType>(State_Type); 
			break;
		case TK_STRING: 
			Sym = ParseString(State_Type); 
			break;
		case TK_ARRAY: 
			Sym = ParseArray(State_Type);
			break;
		case TK_IDENTIFIER: 
			Sym = ParseIdentifier(State_Type, "", Lex.Get().Source); 
			break;
		}
		Lex.AssertAndNext(TK_SEMICOLON);
		Table.Add(new SymType(State_Type, NameNew, Sym));
	}
}

Symbol* Parser::ParseArray(SymState State, string Name){
	Lex.Next();
	Symbol* Sym;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Expr* Exp_Left = ParseExpr();
		AssertConstExpr(Exp_Left);
		Lex.AssertAndNext(TK_DOUBLE_POINT);
		Expr* Exp_Right = ParseExpr();
		AssertConstExpr(Exp_Right);
		Lex.AssertAndNext(TK_CLOSE_SQUARE_BRACKET);
		Lex.AssertAndNext(TK_OF);
		auto Sym = ParseArrayOF<SymArray>(State, Name, Exp_Left, Exp_Right);
		if (Lex.Get().Type == TK_EQUAL && Name != ""){
			auto VecExp = ParseEqual();
			Sym->InitIdent(VecExp);
		}
		return Sym;
	}
	if (State == State_Const){
		throw ExpectedConstExp();
	}
	Lex.AssertAndNext(TK_OF);
	return ParseArrayOF<SymDynArray>(State, Name);
}

template <class ArrayType> Symbol* Parser::ParseArrayOF(SymState State, string Name, Expr* Exp_Left, Expr* Exp_Right){
	if (sym_types.find(Lex.Get().Type) == sym_types.cend() && Table.Find(Lex.Get().Source) == -1){
		throw UnknownType(Lex.Get().Source);
	}
	switch (Lex.Get().Type){
	case TK_INTEGER:
		return new ArrayType(State, Name, ParseSimpleType<SymIntType>(), Exp_Left, Exp_Right);
	case TK_REAL:
		return new ArrayType(State, Name, ParseSimpleType<SymRealType>(), Exp_Left, Exp_Right);
	case TK_CHAR:
		return new ArrayType(State, Name, ParseSimpleType<SymCharType>(), Exp_Left, Exp_Right);
	case TK_BOOLEAN:
		return new ArrayType(State, Name, ParseSimpleType<SymBoolType>(), Exp_Left, Exp_Right);
	case TK_STRING:
		return new ArrayType(State, Name, ParseString(), Exp_Left, Exp_Right);
	case TK_ARRAY:
		return new ArrayType(State, Name, ParseArray(), Exp_Left, Exp_Right);
	case TK_IDENTIFIER:
		return new ArrayType(State, Name, ParseIdentifier(State_NULL, "", Lex.Get().Source), Exp_Left, Exp_Right);
	}
}

Symbol* Parser::ParseString(SymState State, string Name){
	Lex.Next();
	Expr* Exp_Length = nullptr;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Exp_Length = ParseExpr();
		AssertConstExpr(Exp_Length);
		Lex.Assert(TK_CLOSE_SQUARE_BRACKET);
		Lex.Next();
	}
	if (Lex.Get().Type == TK_EQUAL && Name != ""){
		auto VecExp = ParseEqual();
		return new SymStringType(State, Name, VecExp, Exp_Length);
	}
	return new SymStringType(State, Name, vector<Expr*>(), Exp_Length);
}

template <class Sym_X_Type> Symbol* Parser::ParseSimpleType(SymState State, string Name){
	Lex.Next();
	Expr* Exp = nullptr;
	if (Lex.Get().Type == TK_EQUAL && Name != ""){
		Lex.Next();
		Exp = ParseExpr();
	}
	if (Exp == nullptr && State == State_Const){
		throw UnexpectedSymbol("=", Lex.Get().Source);
	}
	return new Sym_X_Type(State, Name, Exp);
}

vector<Expr*> Parser::ParseEqual(){
	Lex.Next();
	vector<Expr*> Vec;
	Expr* Exp;
	int count = 0;
	if (Lex.Get().Type == TK_OPEN_BRACKET){
		do {
			while (Lex.Get().Type == TK_OPEN_BRACKET){
				Lex.Next();
				++count;
			}
			Exp = ParseExpr();
			AssertConstExpr(Exp);
			Vec.push_back(Exp);
			while (Lex.Get().Type == TK_COMMA) {
				Lex.Next();
				Exp = ParseExpr();
				AssertConstExpr(Exp);
				Vec.push_back(Exp);
			} 
			while (Lex.Get().Type == TK_CLOSE_BRACKET && count > 0){
				Lex.Next();
				--count;
			}
			if (Lex.Get().Type == TK_COMMA){
				Lex.Next();
				Lex.Assert(TK_OPEN_BRACKET);
			}
		} while (Lex.Get().Type == TK_OPEN_BRACKET);
		return Vec;
	}
	Exp = ParseExpr();
	AssertConstExpr(Exp);
	Vec.push_back(Exp);
	return Vec;
}

/* Parse Expr */

Expr* Parser::ParseExpr(){
	auto Left = ParseLevel(0);
	auto TK = Lex.Get();
	if (TK.Type == TK_ASSIGNED){
		Lex.Next();
		auto Right = ParseLevel(0);
		if (Left_Op_Assign.find(Left->TypeExp) == Left_Op_Assign.cend()){
			throw ExpectedVariable();
		}
		return new Assign(Left, Right);
	}
	return Left;
}

Symbol* Parser::ParseIdentifier(SymState State, string Name, string TypeName){
	Lex.Next();
	vector<Expr*> VecExp = vector<Expr*>();
	if (Lex.Get().Type == TK_EQUAL && Name != ""){
		VecExp = ParseEqual();
	}
	return new SymTypeID(State, Name, VecExp, TypeName);
}

void Parser::AssertConstExpr(Expr* Exp){
	auto List = new ExpArgList();
	Exp->GetIdentStr(List);
	if (List->Flag == false){
		throw ExpectedConstExp();
	}
	for (int i = 0; i < List->Vec.size(); ++i){
		if (Table.Find(List->Vec[i], State_Const) == -1){
			throw ExpectedConstExp();
		}
	}
}

Expr* Parser::ParseLevel(const int level){
	if (level == parse_factor_level){
		return ParseFactor();
	}
	auto Left = ParseLevel(level + 1);
	while (level_list[level].find(Lex.Get().Type) != level_list[level].cend()){
		Token Op = Lex.Get();
		Lex.Next();
		Left = new ExprBinOp(Left, Op, ParseLevel(level + 1));
	}
	return Left;
}

Expr* Parser::ParseFactor(){
	auto TK = Lex.Get();
	if (TK.Type == NOT_TOKEN){
		throw IllegalExpr();
	}
	if (TK.Type == TK_IDENTIFIER){
		return ParseDesignator();
	}
	Lex.Next();
	if (TK.Type == TK_MINUS || TK.Type == TK_PLUS){
		return new ExprUnarOp(TK, ParseLevel(0));
	}
	if (TK.Type == TK_OPEN_BRACKET){
		auto ExpNow = ParseLevel(0);
		if (Lex.Get().Type != TK_CLOSE_BRACKET){
			throw UnexpectedSymbol(")", Lex.Get().Source);
		}
		Lex.Next();
		return ExpNow;
	}
	if (TK.Type == TK_INTEGER_VALUE){
		return new ExprIntConst(TK);
	}
	if (TK.Type == TK_REAL_VALUE){
		return new ExprRealConst(TK);
	}
	if (TK.Type == TK_TRUE || TK.Type == TK_FALSE){
		return new ExprBoolConst(TK);
	}
	if (TK.Type == TK_STRING_VALUE){
		return new ExprStringConst(TK);
	}
	if (TK.Type == TK_NOT){
		return new ExprUnarOp(TK, ParseLevel(0));
	}
	throw IllegalExpr();
}

Expr* Parser::ParseDesignator(){
	auto TK = Lex.Get();
	Lex.Next();
	Expr* ExpNow = new ExprVar(TK);
	while (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET || Lex.Get().Type == TK_OPEN_BRACKET || Lex.Get().Type == TK_POINT){
		if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
			do {
				Lex.Next();
				ExpNow = new ArrayIndex(ExpNow, ParseLevel(0));
			} while (Lex.Get().Type == TK_COMMA);
			if (Lex.Get().Type != TK_CLOSE_SQUARE_BRACKET){
				throw UnexpectedSymbol("]", Lex.Get().Source);
			}
			Lex.Next();
		}
		if (Lex.Get().Type == TK_OPEN_BRACKET){
			vector<Expr*> Arguments;
			Lex.Next();
			if (Lex.Get().Type != TK_CLOSE_BRACKET){ /* ")" == no argument */
				Arguments.push_back(ParseLevel(0));
				while (Lex.Get().Type == TK_COMMA){
					Lex.Next();
					Arguments.push_back(ParseLevel(0));
				} 
			}
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw UnexpectedSymbol(")", Lex.Get().Source);
			}
			Lex.Next();
			ExpNow = new Function(ExpNow, Arguments);
		}
		if (Lex.Get().Type == TK_POINT){
			Lex.Next();
			if (Lex.Get().Type != TK_IDENTIFIER){
				throw UnexpectedSymbol("identifier", Lex.Get().Source);
			}
			auto Right = new ExprVar(Lex.Get());
			Lex.Next();
			ExpNow = new Record(ExpNow, Right);
		}
	}
	return ExpNow;
}