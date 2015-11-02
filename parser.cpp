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
		Lex.Check(TK_BEGIN);
		Lex.Next();
		Stmt = new Stmt_Compound();
		((Stmt_Compound*)Stmt)->StmtList = ParseStmtList(0);
		Lex.CheckAndNext(TK_END);
		Lex.CheckAndNext(TK_POINT);
		break;
	}
}

/* Parse Statements */

Statement* Parser::ParseStatement(int State){
	switch (Lex.Get().Type) {
	case TK_BEGIN:
		return ParseCompoundStmt(State);
	case TK_IF:
		return ParseIfStmt(State);
	case TK_CASE:
		return ParseCase(State);
	case TK_FOR:
		return ParseForStmt(State);
	case TK_WHILE:
		return ParseWhileStmt(State);
	case TK_REPEAT:
		return ParseRepeatStmt(State);
	case TK_TRY:
		return ParseTryStmt(State);
	case TK_GOTO:
		return ParseGOTOStmt(State);
	case TK_BREAK:
		if (State & 2) {
			Lex.Next();
			CheckSemicolon();
			return new Stmt_BREAK();
		}
		throw NotAllowedStmt("BREAK", Lex.Get().Pos);
	case TK_CONTINUE:
		if (State & 2) {
			Lex.Next();
			CheckSemicolon();
			return new Stmt_Continue();
		}
		throw NotAllowedStmt("CONTINUE", Lex.Get().Pos);
	case TK_RAISE:
		Lex.Next();
		return new Stmt_Raise(ParseExpr());
	case TK_ELSE:
	case TK_END:
	case TK_EXCEPT:
	case TK_FINALLY:
	case TK_UNTIL:
		return nullptr;
	case TK_SEMICOLON:
		Lex.Next();
		return nullptr;
	case TK_IDENTIFIER:
		Exp = ParseExpr();
		CheckType(&Table, Exp, Lex.Get().Pos);
		CheckSemicolon();
		return new Stmt_Assign(Exp);
	default:
		throw UnexpectedSymbol("BEGIN", Lex.Get().Source, Lex.Get().Pos);
	}
}

Statement* Parser::ParseCompoundStmt(int State){
	Lex.CheckAndNext(TK_BEGIN);
	Stmt_Compound* CompStmt = new Stmt_Compound();
	CompStmt->StmtList = ParseStmtList(State);
	Lex.CheckAndNext(TK_END);
	CheckSemicolon();
	return CompStmt;
}

Statement* Parser::ParseGOTOStmt(int State){
	Lex.NextAndCheck(TK_IDENTIFIER);
	auto Sym = Table.GetSymbol(Lex.Get().Source, Lex.Get().Pos);
	Lex.Next();
	CheckSemicolon();
	return new Stmt_GOTO(Sym);
}

Statement* Parser::ParseIfStmt(int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Exp = ParseExpr();
	CheckType(&Table, TypeID_Boolean, Exp, Pos);
	Lex.CheckAndNext(TK_THEN);
	auto Stmt_1 = ParseStatement(State);
	Statement* Stmt_2 = nullptr;
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_2 = ParseStatement(State);
	}
	return new Stmt_IF(Exp, Stmt_1, Stmt_2);
}

Statement* Parser::ParseCase(int State){
	Lex.Next();
	auto Exp = ParseExpr();
	auto Stmt_CASE = new Stmt_Case(Exp);
	Lex.CheckAndNext(TK_OF);
	if (Lex.Get().Type == TK_ELSE || Lex.Get().Type == TK_END) {
		throw IllegalExpr(Lex.Get().Pos);
	}
	while (Lex.Get().Type != TK_ELSE && Lex.Get().Type != TK_END) {
		auto Pos = Lex.Get().Pos;
		Expr* Exp_1 = ParseExpr();
		CheckType(&Table, Exp, Exp_1, Pos);
		CheckConstExpr(Exp_1);
		Expr* Exp_2 = nullptr;
		if (Lex.Get().Type == TK_DOUBLE_POINT) {
			Lex.Next();
			Pos = Lex.Get().Pos;
			Exp_2 = ParseExpr();
			CheckType(&Table, Exp, Exp_2, Pos);
			CheckConstExpr(Exp_2);
		}
		Lex.CheckAndNext(TK_COLON);
		auto Stmt = ParseStatement(State | 4);
		Stmt_CASE->Add(Case_Selector(Exp_1, Exp_2, Stmt));
	}
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_CASE->Stmt_Else = ParseStatement(State);
	}
	Lex.CheckAndNext(TK_END);
	CheckSemicolon();
	return Stmt_CASE;
}

Statement* Parser::ParseForStmt(int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Exp_1 = ParseExpr();
	CheckType(&Table, Exp_1, Pos);
	bool isTO;
	if (Lex.Get().Type == TK_DOWNTO){
		Lex.Next();
		isTO = false;
	}
	else {
		Lex.CheckAndNext(TK_TO);
		isTO = true;
	}
	Pos = Lex.Get().Pos;
	auto Exp_2 = ParseExpr();
	CheckType(&Table, TypeID_Integer, Exp_2, Pos);
	Lex.CheckAndNext(TK_DO);
	auto Stmt = ParseStatement(State | 2);
	if (Stmt != nullptr) {
		CheckSemicolon();
	}
	return new Stmt_FOR(Exp_1, Exp_2, isTO, Stmt);
}

Statement* Parser::ParseWhileStmt(int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Cond = ParseExpr();
	CheckType(&Table, TypeID_Boolean, Cond, Pos);
	Lex.CheckAndNext(TK_DO);
	return new Stmt_WHILE(Cond, ParseStatement(State | 2));
}

Statement* Parser::ParseRepeatStmt(int State){
	Lex.Next();
	auto Stmt_List = ParseStmtList(State);
	Lex.CheckAndNext(TK_UNTIL);
	auto Pos = Lex.Get().Pos;
	auto Exp = ParseExpr();
	CheckType(&Table, TypeID_Boolean, Exp, Pos);
	return new Stmt_REPEAT(Exp, Stmt_List);
}

Statement* Parser::ParseTryStmt(int State){
	Lex.Next();
	auto Stmt_1 = ParseStmtList(State);
	vector<Statement*> Stmt_2;
	switch (Lex.Get().Type){
	case TK_EXCEPT:
		Lex.Next();
		Stmt_2 = ParseStmtList(State);
		Lex.CheckAndNext(TK_END);
		return new Stmt_Try_Except(Stmt_1, Stmt_2);
		break;
	case TK_FINALLY:
		Lex.Next();
		Stmt_2 = ParseStmtList(State);
		Lex.CheckAndNext(TK_END);
		return new Stmt_Try_Finally(Stmt_1, Stmt_2);
		break;
	default:
		throw UnexpectedSymbol("EXCEPT", Lex.Get().Source, Lex.Get().Pos);
	}
}

set<TokenType> end_block_tk = { TK_END, TK_EXCEPT, TK_FINALLY, TK_ELSE, TK_UNTIL, TK_POINT };

vector<Statement*> Parser::ParseStmtList(int State) {
	vector<Statement*> Ans;
	while (end_block_tk.find(Lex.Get().Type) == end_block_tk.cend()){
		auto Stmt = ParseStatement(State);
		if (Stmt != nullptr) {
			Ans.push_back(Stmt);
		}
	}
	return Ans;
}

void Parser::CheckSemicolon() {
	if (end_block_tk.find(Lex.Get().Type) == end_block_tk.cend()) {
		Lex.CheckAndNext(TK_SEMICOLON);
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
			throw UnexpectedSymbol("BEGIN", Lex.Get().Source, Lex.Get().Pos);
		}
	}
}

void Parser::ParseLabelDecl(){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		Table.CheckSymbol(Lex.Get().Source, Lex.Get().Pos);
		Table.Add(new SymLabel(Lex.Get().Source));
		Lex.Next();
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseConstDecl(){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Name = Lex.Get().Source;
		auto Name_Pos = Lex.Get().Pos;
		Lex.Next(); 
		Symbol* Type = nullptr;
		if (Lex.Get().Type == TK_COLON) {
			Lex.Next();
			Type = ParseType();
			while (((SymType*)Type)->TypeID == TypeID_BadType) {
				Type = ((SymType*)Type)->Type;
			}
		}
		Lex.Check(TK_EQUAL);
		auto Pos = Lex.Get().Pos;
		auto Exp = ParseEqual();
		if (Exp->TypeExp == InitExp && Type == nullptr) {
			throw IllegalExpr(Pos);
		}
		if (Type != nullptr) {
			CheckType(&Table, Type, Exp, Pos);
		}
		else {
			Type = new SymType("", CheckType(&Table, Pos).GetTypeID(Exp));
		}
		Table.CheckSymbol(Name, Name_Pos);
		Table.Add(new SymConst(Name, Exp, Type));
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

//void Parser::ParseFunction(){}
//void Parser::ParseProcedure(){}

void Parser::ParseVarDecl(){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		vector<string> Names;
		Names.push_back(Lex.Get().Source);
		vector<Position> Names_Pos;
		Names_Pos.push_back(Lex.Get().Pos);
		Lex.Next();
		while (Lex.Get().Type == TK_COMMA) {
			Lex.NextAndCheck(TK_IDENTIFIER);
			Names.push_back(Lex.Get().Source);
			Names_Pos.push_back(Lex.Get().Pos);
			Lex.Next();
		}
		Lex.CheckAndNext(TK_COLON);
		auto Type = ParseType();
		while (((SymType*)Type)->TypeID == TypeID_BadType) {
			Type = ((SymType*)Type)->Type;
		}
		if (Names.size() > 1 && Lex.Get().Type == TK_EQUAL) {
			throw UnexpectedSymbol(";", "=", Lex.Get().Pos);
		}
		Expr* Exp = nullptr;
		if (Lex.Get().Type == TK_EQUAL) {
			auto Pos = Lex.Get().Pos;
			Exp = ParseEqual();
			CheckType(&Table, Type, Exp, Pos);
		}
		for (int i = 0; i < Names.size(); ++i) {
			Table.CheckSymbol(Names[i], Names_Pos[i]);
			Table.Add(new SymVar(Names[i], Exp, Type));
		}
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseTypeDecl(){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		string NameNew = Lex.Get().Source;
		Position NameNew_Pos = Lex.Get().Pos;
		Lex.NextAndCheck(TK_EQUAL);
		Lex.Next();
		Table.CheckSymbol(NameNew, NameNew_Pos);
		Table.Add(new SymType(NameNew, ParseType()));
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

Symbol* Parser::ParseArray(){
	Lex.Next();
	Symbol* Sym;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Expr* Exp_Left = ParseExpr(); 
		CheckConstExpr(Exp_Left);
		int A = CalculateConstExpr<int>(&Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Left);
		Lex.CheckAndNext(TK_DOUBLE_POINT);
		Expr* Exp_Right = ParseExpr();
		CheckConstExpr(Exp_Right);
		int B = CalculateConstExpr<int>(&Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Right);
		SymArray* Sym = new SymArray(nullptr, A, B);
		Symbol** Sym_TypeInit = &Sym->Type;
		while (Lex.Get().Type == TK_COMMA){
			Lex.Next();
			Exp_Left = ParseExpr();
			CheckConstExpr(Exp_Left);
			A = CalculateConstExpr<int>(&Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Left);
			Lex.CheckAndNext(TK_DOUBLE_POINT);
			Exp_Right = ParseExpr();
			CheckConstExpr(Exp_Right);
			B = CalculateConstExpr<int>(&Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Right);
			*Sym_TypeInit = new SymArray(nullptr, A, B);
			Sym_TypeInit = &((SymArray*)*Sym_TypeInit)->Type;
		}
		Lex.CheckAndNext(TK_CLOSE_SQUARE_BRACKET);
		Lex.CheckAndNext(TK_OF);
		auto Pos = Lex.Get().Pos;
		*Sym_TypeInit = ParseType();
		while (((SymType*)*Sym_TypeInit)->TypeID == TypeID_BadType) {
			auto Sym = Table.GetSymbol((*Sym_TypeInit)->Name, Pos);
			*Sym_TypeInit = ((SymType*)Sym)->Type;
		}
		return Sym;
	}
	Lex.CheckAndNext(TK_OF);
	return new SymDynArray(ParseType());
}

Symbol* Parser::ParseString(){
	Lex.Next();
	int Length = -1;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Position Pos = Lex.Get().Pos;
		auto Exp_Length = ParseExpr();
		CheckConstExpr(Exp_Length);
		Length = CalculateConstExpr<int>(&Table, "INTEGER", Pos).Calculate(Exp_Length);
		Lex.Check(TK_CLOSE_SQUARE_BRACKET);
		Lex.Next();
	}
	return new SymStringType(Length);
}

Expr* Parser::ParseInitList(){
	InitList* Ans = new InitList();
	do {
		Lex.Next();
		if (Lex.Get().Type == TK_OPEN_BRACKET) {
			Ans->List.push_back(ParseInitList());
			Lex.CheckAndNext(TK_CLOSE_BRACKET);
			if (Lex.Get().Type != TK_COMMA) {
				return Ans;
			}
			continue;
		}
		auto Exp = ParseExpr();
		CheckConstExpr(Exp);
		Ans->List.push_back(Exp);
	} while (Lex.Get().Type == TK_COMMA);
	return Ans;
}

Expr* Parser::ParseEqual(){
	Lex.Next();
	int count = 0;
	if (Lex.Get().Type == TK_OPEN_BRACKET){
		auto Exp = ParseInitList();
		Lex.CheckAndNext(TK_CLOSE_BRACKET);
		return Exp;
	}
	auto Exp = ParseExpr();
	CheckConstExpr(Exp);
	return Exp;
}

Symbol* Parser::ParseType(){
	switch (Lex.Get().Type){
	case TK_STRING:
		return ParseString();
	case TK_ARRAY:
		return ParseArray();
	default:
		auto Pos = Lex.Get().Pos;
		if (Table.Find(Lex.Get().Source) == -1) {
			throw UnknownType(Lex.Get().Source, Lex.Get().Pos);
		}
		auto Sym = Table.GetSymbol(Lex.Get().Source, Lex.Get().Pos);
		while (((SymType*)Sym)->TypeID == TypeID_BadType) { 
			auto _Sym = Table.GetSymbol(Sym->Name, Pos);
			Sym = ((SymType*)_Sym)->Type;
		}
		Lex.Next();
		return Sym;
	}
}

void Parser::CheckConstExpr(Expr* Exp) {
	auto List = new ExpArgList();
	Exp->GetIdentStr(List);
	if (List->Flag == false) {
		throw ExpectedConstExp(Lex.Get().Pos);
	}
	for (int i = 0; i < List->Vec.size(); ++i) {
		if (Table.Find(List->Vec[i]) != -1 && Table.GetSymbol(List->Vec[i], Lex.Get().Pos)->Section != DeclConst) {
			throw ExpectedConstExp(Lex.Get().Pos);
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
			throw ExpectedVariable(Lex.Get().Pos);
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
		throw IllegalExpr(TK.Pos);
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
			throw UnexpectedSymbol(")", Lex.Get().Source, TK.Pos);
		}
		Lex.Next();
		return ExpNow;
	}
	if (TK.Type == TK_INTEGER_VALUE){
		return new ExprIntConst(TK);
	}
	if (TK.Type == TK_REAL_VALUE){
		return new ExprDoubleConst(TK);
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
	throw IllegalExpr(TK.Pos);
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
				throw UnexpectedSymbol("]", Lex.Get().Source, Lex.Get().Pos);
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
				throw UnexpectedSymbol(")", Lex.Get().Source, Lex.Get().Pos);
			}
			Lex.Next();
			ExpNow = new Function(ExpNow, Arguments);
		}
		if (Lex.Get().Type == TK_POINT){
			Lex.Next();
			if (Lex.Get().Type != TK_IDENTIFIER){
				throw UnexpectedSymbol("identifier", Lex.Get().Source, Lex.Get().Pos);
			}
			auto Right = new ExprVar(Lex.Get());
			Lex.Next();
			ExpNow = new Record(ExpNow, Right);
		}
	}
	return ExpNow;
}