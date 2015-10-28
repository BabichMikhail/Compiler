#include "parser.h"
#include "errors.h"
#include <type_traits>
#include "calculate.h"

using namespace std;

#define parse_factor_level 3

set<TokenType> start_expr_tk = { TK_INTEGER_VALUE, TK_REAL_VALUE, TK_OPEN_BRACKET, TK_IDENTIFIER, TK_PLUS, TK_MINUS, TK_TRUE, TK_FALSE, TK_NOT, TK_STRING_VALUE };

set<TokenType> rel_op = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };
set<TokenType> add_op = { TK_PLUS, TK_MINUS, TK_OR, TK_XOR };
set<TokenType> mul_op = { TK_MUL, TK_DIV, TK_DIV_INT, TK_MOD, TK_AND, TK_SHL, TK_SHR };
set<TokenType> simp_stat = { TK_OPEN_SQUARE_BRACKET, TK_OPEN_BRACKET, TK_POINT };
vector<set<TokenType>> level_list = { rel_op, add_op, mul_op };

set<TypeExpr> Left_Op_Assign = { VarExp, RecordExp, ArrayExp };

void Parser::Print(){
	switch (State) {
	case Test_Exp:
		if (Exp != nullptr)
			Exp->Print(0);
		break;
	case Test_Decl:
		Table.Print();
		break;
	case Test_Statement:
		Stmt->Print(0);
		break;
	}
}

Parser::Parser(const char* filename, PMod State) : Lex(filename), State(State){
	switch (State) {
	case Test_Exp:
		while (Lex.isToken()) {
			Lex.Next();
			if (start_expr_tk.find(Lex.Get().Type) != start_expr_tk.end()) {
				Exp = ParseExpr();
			}
		}
		break;
	case Test_Decl:
		ParseDeclSection();
		break;
	case Test_Statement:
		ParseDeclSection();
		Stmt = ParseStatement();
		Lex.AssertAndNext(TK_POINT);
		break;
	}
}

/* Parse Statements */

Statement* Parser::ParseStatement(){
	switch (Lex.Get().Type) {
	case TK_BEGIN:
		return ParseCompoundStmt();
		break;
	case TK_IF:
		return ParseIfStmt();
		break;
	case TK_CASE:
		return ParseCase();
		break;
	case TK_FOR:
		return ParseForStmt();
		break;
	case TK_WHILE:
		return ParseWhileStmt();
		break;
	case TK_REPEAT:
		return ParseRepeatStmt();
		break;
	case TK_TRY:
		return ParseTryStmt();
		break;
	case TK_GOTO:
		return ParseGOTOStmt();
		break;
	case TK_BREAK:
		return new Stmt_BREAK();
	default:
		throw UnexpectedSymbol("BEGIN", Lex.Get().Source);
	}
}

Statement* Parser::ParseCompoundStmt(){
	Lex.AssertAndNext(TK_BEGIN);
	Stmt_Compound* CompStmt = new Stmt_Compound();
	while (Lex.Get().Type != TK_END) {
		CompStmt->Add(ParseStatement());
	}
	Lex.Next();
	return CompStmt;
}

Statement* Parser::ParseGOTOStmt(){
	Lex.Next();
	Lex.AssertAndNext(TK_IDENTIFIER);
	return new Stmt_GOTO(Table.GetSymbol(Lex.Get().Source));
}

Statement* Parser::ParseIfStmt(){
	Lex.Next();
	auto Exp = ParseExpr();
	Lex.AssertAndNext(TK_THEN);
	auto Stmt_1 = ParseStatement();
	Statement* Stmt_2 = nullptr;
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_2 = ParseStatement();
	}
	return new Stmt_IF(Exp, Stmt_1, Stmt_2);
}

Statement* Parser::ParseCase(){
	Lex.Next();
	auto Stmt_CASE = new Stmt_Case(ParseExpr());
	Lex.AssertAndNext(TK_OF);
	while (Lex.Get().Type != TK_ELSE && Lex.Get().Type != TK_END) {
		Expr* Exp_1 = ParseExpr();
		AssertConstExpr(Exp_1);
		Expr* Exp_2 = nullptr;
		if (Lex.Get().Type == TK_DOUBLE_POINT) {
			Lex.Next();
			Exp_2 = ParseExpr();
			AssertConstExpr(Exp_2);
		}
		Lex.AssertAndNext(TK_COLON);
		auto Stmt = ParseStatement();
		Lex.AssertAndNext(TK_SEMICOLON);
		Stmt_CASE->Add(Case_Selector(Exp_1, Exp_2, Stmt));
	}
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_CASE->Stmt_Else = ParseStatement();
	}
	if (Lex.Get().Type == TK_SEMICOLON) {
		Lex.Next();
	}
	return Stmt_CASE;
}

Statement* Parser::ParseForStmt(){
	Lex.Next();
	auto Exp_1 = ParseExpr();
	bool isTO;
	if (Lex.Get().Type == TK_DOWNTO){
		Lex.Next();
		isTO = false;
	}
	else {
		Lex.AssertAndNext(TK_TO);
		isTO = true;
	}
	auto Exp_2 = ParseExpr();
	Lex.AssertAndNext(TK_DO);
	return new Stmt_FOR(Exp_1, Exp_2, isTO, ParseStatement());
}

Statement* Parser::ParseWhileStmt(){
	Lex.Next();
	auto Cond = ParseExpr();
	Lex.AssertAndNext(TK_DO);
	return new Stmt_WHILE(Cond, ParseStatement());
}

Statement* Parser::ParseRepeatStmt(){
	Lex.Next();
	auto ParseStatement();
	Lex.AssertAndNext(TK_UNTIL);
	auto Exp = ParseExpr();
	return new Stmt_REPEAT(Exp, Stmt);
}

Statement* Parser::ParseTryStmt(){
	Lex.Next();
	auto Stmt_1 = ParseStatement();
	Statement* Stmt_2 = nullptr;
	switch (Lex.Get().Type){
	case TK_EXCEPT:
		Stmt_2 = ParseStatement();
		Lex.AssertAndNext(TK_END);
		return new Stmt_Try_Except(Stmt_1, Stmt_2);
		break;
	case TK_FINALLY:
		Stmt_2 = ParseStatement();
		Lex.AssertAndNext(TK_END);
		return new Stmt_Try_Finally(Stmt_1, Stmt_2);
		break;
	default:
		throw UnexpectedSymbol("EXCEPT", Lex.Get().Source);
	}
}

/* Parse Declarations */

void Parser::ParseDeclSection(){
	Lex.Next();
	while (Lex.isToken()){
		switch (Lex.Get().Type){
		case TK_LABEL:
			ParseLabelDecl();
			break;
		case TK_CONST:
			ParseConstDecl();
			break;
		case TK_TYPE:
			ParseTypeDecl();
			break;
			//case TK_FUNCTION:
			//	ParseFunction();
			//	break;
			//case TK_PROCEDURE:
			//	ParseProcedure();
			//	break;
		case TK_VAR:
			ParseVarDecl();
			break;
		case TK_BEGIN:
			return;
		default:
			throw UnexpectedSymbol("BEGIN", Lex.Get().Source);
		}
	}
}

void Parser::ParseLabelDecl(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		Table.Add(new SymLabel(Lex.Get().Source));
		Lex.Next();
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseConstDecl(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Name = Lex.Get().Source;
		Lex.Next(); 
		Symbol* Type = nullptr;
		if (Lex.Get().Type == TK_COLON) {
			Lex.Next();
			Type = ParseType();
		}
		Lex.Assert(TK_EQUAL);
		Table.Add(new SymConst(Name, ParseEqual(), Type));
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

//void Parser::ParseFunction(){}
//void Parser::ParseProcedure(){}

void Parser::ParseVarDecl(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		vector<string> Names;
		Names.push_back(Lex.Get().Source);
		Lex.Next();
		while (Lex.Get().Type == TK_COMMA) {
			Lex.NextAndAssert(TK_IDENTIFIER);
			Names.push_back(Lex.Get().Source);
			Lex.Next();
		}
		Lex.AssertAndNext(TK_COLON);
		auto Type = ParseType();
		if (Names.size() > 1 && Lex.Get().Type == TK_EQUAL) {
			throw UnexpectedSymbol(";", "=");
		}
		for (int i = 0; i < Names.size(); ++i) {
			Table.Add(new SymVar(Names[i], Lex.Get().Type == TK_EQUAL ? ParseEqual() : vector<InitExpr*>(), Type));
		}
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseTypeDecl(){
	Lex.NextAndAssert(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		string NameNew = Lex.Get().Source;
		Lex.NextAndAssert(TK_EQUAL);
		Lex.Next();
		Table.Add(new SymType(NameNew, ParseType()));
		Lex.AssertAndNext(TK_SEMICOLON);
	}
}

Symbol* Parser::ParseArray(){
	Lex.Next();
	Symbol* Sym;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Expr* Exp_Left = ParseExpr(); 
		AssertConstExpr(Exp_Left);
		int A = CalculateConstExpr<int>(&Table).Calculate(Exp_Left);
		Lex.AssertAndNext(TK_DOUBLE_POINT);
		Expr* Exp_Right = ParseExpr();
		AssertConstExpr(Exp_Right);
		int B = CalculateConstExpr<int>(&Table).Calculate(Exp_Right);
		SymArray* Sym = new SymArray(nullptr, A, B);
		Symbol** Sym_TypeInit = &Sym->Type;
		while (Lex.Get().Type == TK_COMMA){
			Lex.Next();
			Exp_Left = ParseExpr();
			AssertConstExpr(Exp_Left);
			A = CalculateConstExpr<int>(&Table).Calculate(Exp_Left);
			Lex.AssertAndNext(TK_DOUBLE_POINT);
			Exp_Right = ParseExpr();
			AssertConstExpr(Exp_Right);
			B = CalculateConstExpr<int>(&Table).Calculate(Exp_Right);
			*Sym_TypeInit = new SymArray(nullptr, A, B);
			Sym_TypeInit = &((SymArray*)*Sym_TypeInit)->Type;
		}
		Lex.AssertAndNext(TK_CLOSE_SQUARE_BRACKET);
		Lex.AssertAndNext(TK_OF);
		*Sym_TypeInit = ParseType();
		return Sym;
	}
	Lex.AssertAndNext(TK_OF);
	return new SymDynArray(ParseType());
}

Symbol* Parser::ParseString(){
	Lex.Next();
	int Length = -1;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		auto Exp_Length = ParseExpr();
		AssertConstExpr(Exp_Length);
		Length = CalculateConstExpr<int>(&Table).Calculate(Exp_Length);
		Lex.Assert(TK_CLOSE_SQUARE_BRACKET);
		Lex.Next();
	}
	return new SymStringType(Length);
}

vector<InitExpr*> Parser::ParseEqual(){
	Lex.Next();

	vector<InitExpr*> Vec;
	int count = 0;
	if (Lex.Get().Type == TK_OPEN_BRACKET){
		do {
			while (Lex.Get().Type == TK_OPEN_BRACKET){
				Lex.Next();
				++count;
			}
			Exp = ParseExpr();
			AssertConstExpr(Exp);
			Vec.push_back(new InitExpr(Exp, count));
			while (Lex.Get().Type == TK_COMMA) {
				Lex.Next();
				Exp = ParseExpr();
				AssertConstExpr(Exp);
				Vec.push_back(new InitExpr(Exp, count));
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
	Vec.push_back(new InitExpr(Exp, 0));
	return Vec;
}

Symbol* Parser::ParseType(){
	switch (Lex.Get().Type){
	case TK_STRING:
		return ParseString();
	case TK_ARRAY:
		return ParseArray();
	default:
		if (Table.Find(Lex.Get().Source) == -1) {
			throw UnknownType(Lex.Get().Source);
		}
		auto Sym = Table.GetSymbol(Lex.Get().Source);
		Lex.Next();
		return Sym;
	}
}

void Parser::AssertConstExpr(Expr* Exp) {
	auto List = new ExpArgList();
	Exp->GetIdentStr(List);
	if (List->Flag == false) {
		throw ExpectedConstExp();
	}
	for (int i = 0; i < List->Vec.size(); ++i) {
		if (Table.Find(List->Vec[i]) != -1 && Table.GetSymbol(List->Vec[i])->GetSection() != DeclConst) {
			throw ExpectedConstExp();
		}
	}
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