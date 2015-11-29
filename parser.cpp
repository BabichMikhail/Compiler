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
		Table->Print(0);
		break;
	case Test_Statement:
		Stmt->Print(0);
		break;
	}
}

void Parser::ReservedCastFunction(SymTable* Table, string Type_1, string Type_2) {
	auto NewTable = new SymTable(Table);
	Symbol* SymArg = new SymVar("Arg_0", nullptr, Table->GetSymbol(Type_2, Position()), RValue);
	NewTable->Add(SymArg);
	Symbol* SymResult = new SymVar("Result", nullptr, Table->GetSymbol(Type_1, Position()), RValue);
	NewTable->Add(SymResult);
	auto Stmt = new Stmt_Compound();
	Stmt->Add(new Stmt_Assign(new ExprAssign(new ExprIdent(NewTable->GetSymbol("Result", Position()), Position()), 
		new ExprIdent(NewTable->GetSymbol("Arg_0", Position()), Position()))));
	
	auto SymFunc = new SymFunction(Type_1, NewTable, new Stmt_Compound(), 2, Table->GetSymbol(Type_1, Position()));
	Table->Add(SymFunc);
	++Table->DeclCount;
}

void Parser::ReservedFunctions(SymTable* Table) {
	ReservedCastFunction(Table, "double", "integer");
	ReservedCastFunction(Table, "integer", "char");
	ReservedCastFunction(Table, "char", "integer");
	Table->Add(new SymProcedure("write", new SymTable(Table), nullptr, argc_write));
	Table->Add(new SymProcedure("writeln", new SymTable(Table), nullptr, argc_writeln));
	++++Table->DeclCount;
}

void Parser::ReservedTypes(SymTable* Table) {
	Table->Symbols.push_back(new SymType("integer", TypeID_Integer));
	Table->Symbols.push_back(new SymType("double", TypeID_Double));
	Table->Symbols.push_back(new SymType("char", TypeID_Char));
	Table->Symbols.push_back(new SymType("boolean", TypeID_Boolean));
	Table->Symbols.push_back(new SymType("string", TypeID_String));
	Table->Symbols.push_back(new SymType("array", TypeID_Array));
	Table->Symbols.push_back(new SymType("record", TypeID_Record));
	Table->Symbols.push_back(new SymType("pointer", TypeID_Pointer));
	Table->DeclCount = Table->Symbols.size();
}

Parser::Parser(const char* filename, PMod State) : Lex(filename), State(State), Table(new SymTable(nullptr)){
	ReservedTypes(Table);
	ReservedFunctions(Table);
	switch (State) {
	case Test_Exp:
		while (Lex.isToken()) {
			Lex.Next();
			if (start_expr_tk.find(Lex.Get().Type) != start_expr_tk.end()) {
				Exp = ParseExpr(Table);
			}
		}
		break;
	case Test_Decl:
		Lex.Next();
		ParseDeclSection(Table);
		break;
	case Test_Statement:
		Lex.Next();
		ParseDeclSection(Table);
		Lex.Check(TK_BEGIN);
		Lex.Next();
		Stmt = new Stmt_Compound();
		((Stmt_Compound*)Stmt)->StmtList = ParseStmtList(Table, 0);
		Lex.CheckAndNext(TK_END);
		Lex.CheckAndNext(TK_POINT);
		break;
	}
}

/* Parse Statements */

Statement* Parser::ParseStatement(SymTable* Table, int State){
	switch (Lex.Get().Type) {
	case TK_BEGIN:
		return ParseCompoundStmt(Table, State);
	case TK_IF:
		return ParseIfStmt(Table, State);
	case TK_CASE:
		return ParseCase(Table, State);
	case TK_FOR:
		return ParseForStmt(Table, State);
	case TK_WHILE:
		return ParseWhileStmt(Table, State);
	case TK_REPEAT:
		return ParseRepeatStmt(Table, State);
	case TK_TRY:
		return ParseTryStmt(Table, State);
	case TK_GOTO:
		return ParseGOTOStmt(Table, State);
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
		return new Stmt_Raise(ParseExpr(Table));
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
		return ParseIdentifier(Table, State);
	default:
		throw UnexpectedSymbol("BEGIN", Lex.Get().Source, Lex.Get().Pos);
	}
}

Statement* Parser::ParseCompoundStmt(SymTable* Table, int State){
	Lex.CheckAndNext(TK_BEGIN);
	Stmt_Compound* CompStmt = new Stmt_Compound();
	CompStmt->StmtList = ParseStmtList(Table, State);
	Lex.CheckAndNext(TK_END);
	CheckSemicolon();
	return CompStmt;
}

Statement* Parser::ParseGOTOStmt(SymTable* Table, int State){
	Lex.NextAndCheck(TK_IDENTIFIER);
	auto Sym = Table->GetSymbol(Lex.Get().Source, Lex.Get().Pos);
	Lex.Next();
	CheckSemicolon();
	return new Stmt_GOTO(Sym);
}

Statement* Parser::ParseIfStmt(SymTable* Table, int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Exp = ParseExpr(Table);
	CheckType(Table, TypeID_Boolean, Exp, Pos);
	Lex.CheckAndNext(TK_THEN);
	auto Stmt_1 = ParseStatement(Table, State);
	Statement* Stmt_2 = nullptr;
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_2 = ParseStatement(Table, State);
	}
	return new Stmt_IF(Exp, Stmt_1, Stmt_2);
}

Statement* Parser::ParseCase(SymTable* Table, int State){
	Lex.Next();
	auto Exp = ParseExpr(Table);
	auto Stmt_CASE = new Stmt_Case(Exp);
	Lex.CheckAndNext(TK_OF);
	if (Lex.Get().Type == TK_ELSE || Lex.Get().Type == TK_END) {
		throw IllegalExpr(Lex.Get().Pos);
	}
	while (Lex.Get().Type != TK_ELSE && Lex.Get().Type != TK_END) {
		auto Pos = Lex.Get().Pos;
		Expr* Exp_1 = ParseExpr(Table);
		CheckType(Table, Exp, Exp_1, Pos);
		CheckConstExpr(Table, Exp_1);
		Expr* Exp_2 = nullptr;
		if (Lex.Get().Type == TK_DOUBLE_POINT) {
			Lex.Next();
			Pos = Lex.Get().Pos;
			Exp_2 = ParseExpr(Table);
			CheckType(Table, Exp, Exp_2, Pos);
			CheckConstExpr(Table, Exp_2);
		}
		Lex.CheckAndNext(TK_COLON);
		auto Stmt = ParseStatement(Table, State);
		Stmt_CASE->Add(Case_Selector(Exp_1, Exp_2, Stmt));
	}
	if (Lex.Get().Type == TK_ELSE) {
		Lex.Next();
		Stmt_CASE->Stmt_Else = ParseStatement(Table, State);
	}
	Lex.CheckAndNext(TK_END);
	CheckSemicolon();
	return Stmt_CASE;
}

Statement* Parser::ParseForStmt(SymTable* Table, int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Exp_1 = ParseExpr(Table);
	CheckType(Table, Exp_1, Pos);
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
	auto Exp_2 = ParseExpr(Table);
	CheckType(Table, TypeID_Integer, Exp_2, Pos);
	Lex.CheckAndNext(TK_DO);
	auto Stmt = ParseStatement(Table, State | 2);
	if (Stmt != nullptr) {
		CheckSemicolon();
	}
	return new Stmt_FOR(Exp_1, Exp_2, isTO, Stmt);
}

Statement* Parser::ParseWhileStmt(SymTable* Table, int State){
	Lex.Next();
	auto Pos = Lex.Get().Pos;
	auto Cond = ParseExpr(Table);
	CheckType(Table, TypeID_Boolean, Cond, Pos);
	Lex.CheckAndNext(TK_DO);
	return new Stmt_WHILE(Cond, ParseStatement(Table, State | 2));
}

Statement* Parser::ParseRepeatStmt(SymTable* Table, int State){
	Lex.Next();
	auto Stmt_List = ParseStmtList(Table, State);
	Lex.CheckAndNext(TK_UNTIL);
	auto Pos = Lex.Get().Pos;
	auto Exp = ParseExpr(Table);
	CheckType(Table, TypeID_Boolean, Exp, Pos);
	return new Stmt_REPEAT(Exp, Stmt_List);
}

Statement* Parser::ParseTryStmt(SymTable* Table, int State){
	Lex.Next();
	auto Stmt_1 = ParseStmtList(Table, State);
	vector<Statement*> Stmt_2;
	switch (Lex.Get().Type){
	case TK_EXCEPT:
		Lex.Next();
		Stmt_2 = ParseStmtList(Table, State);
		Lex.CheckAndNext(TK_END);
		return new Stmt_Try_Except(Stmt_1, Stmt_2);
		break;
	case TK_FINALLY:
		Lex.Next();
		Stmt_2 = ParseStmtList(Table, State);
		Lex.CheckAndNext(TK_END);
		return new Stmt_Try_Finally(Stmt_1, Stmt_2);
		break;
	default:
		throw UnexpectedSymbol("EXCEPT", Lex.Get().Source, Lex.Get().Pos);
	}
}

Statement* Parser::ParseIdentifier(SymTable* Table, int State) {
	auto Pos = Lex.Get().Pos;
	Exp = ParseExpr(Table);
	if (Exp->TypeExp == FunctionExp) {
		CheckType(Table, TypeID_BadType, Exp, Pos);
		return new Stmt_Call(Exp);
	}
	if (Exp->TypeExp == AssignExp) {
		CheckType(Table, Exp, Pos);
		CheckSemicolon();
		return new Stmt_Assign(Exp);
	}
	throw IllegalExpr(Pos);
}

set<TokenType> end_block_tk = { TK_END, TK_EXCEPT, TK_FINALLY, TK_ELSE, TK_UNTIL, TK_POINT };

vector<Statement*> Parser::ParseStmtList(SymTable* Table, int State) {
	vector<Statement*> Ans;
	while (end_block_tk.find(Lex.Get().Type) == end_block_tk.cend()){
		auto Stmt = ParseStatement(Table, State);
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

void Parser::ParseDeclSection(SymTable* Table){
	while (Lex.isToken()){
		switch (Lex.Get().Type){
		case TK_LABEL:
			ParseLabelDecl(Table);
			break;
		case TK_CONST:
			ParseConstDecl(Table);
			break;
		case TK_TYPE:
			ParseTypeDecl(Table);
			break;
		case TK_FUNCTION:
			ParseCallDecl(Table, DeclFunction);
			break;
		case TK_PROCEDURE:
			ParseCallDecl(Table, DeclProcedure);
			break;
		case TK_VAR:
			ParseVarDecl(Table);
			break;
		case TK_BEGIN:
			return;
		default:
			throw UnexpectedSymbol("BEGIN", Lex.Get().Source, Lex.Get().Pos);
		}
	}
	for (auto i = 0; i < DeclForwardCall.size(); ++i) {
		if (((SymCall*)DeclForwardCall[i].Sym)->Stmt == nullptr) {
			throw ForwardDeclNotSolved(DeclForwardCall[i].Sym->Name, DeclForwardCall[i].Pos);
		}
	}
}

void Parser::ParseLabelDecl(SymTable* Table){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		Table->CheckSymbol(Lex.Get().Source, Lex.Get().Pos);
		Table->Add(new SymLabel(Lex.Get().Source));
		Lex.Next();
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseConstDecl(SymTable* Table){
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER){
		auto Name = Lex.Get().Source;
		auto Name_Pos = Lex.Get().Pos;
		Lex.Next(); 
		Symbol* Type = nullptr;
		if (Lex.Get().Type == TK_COLON) {
			Lex.Next();
			Type = ParseType(Table);
			while (((SymType*)Type)->TypeID == TypeID_BadType) {
				Type = Type->GetType();
			}
		}
		Lex.Check(TK_EQUAL);
		auto Pos = Lex.Get().Pos;
		auto Exp = ParseInit(Table);
		if (Exp->TypeExp == InitExp && Type == nullptr) {
			throw IllegalExpr(Pos);
		}
		ArgState State = RValue;
		if (Type != nullptr) {
			CheckType(Table, Type, Exp, Pos);
			if (_stricmp(Type->Name.c_str(), "pointer") == 0 || Type->Name.length() == 0) {
				State = Var;
			}
		}
		else {
			auto TypeID = CheckType(Table, Pos).GetTypeID(Exp);
			Type = new SymType("", TypeID);
			if (TypeID == TypeID_Array || TypeID == TypeID_Pointer) {
				State = Var;
			}
		}
		
		
		Table->CheckSymbol(Name, Name_Pos);
		Table->Add(new SymConst(Name, Exp, Type, RValue));
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseVarDecl(SymTable* Table){
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
		auto Type = ParseType(Table);
		ArgState State = RValue;
		if (_stricmp(Type->Name.c_str(), "pointer") == 0 || Type->Name.length() == 0) {
			State = RValue;
		}
		while (Type->Section == DeclType && ((SymType*)Type)->TypeID == TypeID_BadType) {
			Type = ((SymType*)Type)->Type;
		}
		if (Names.size() > 1 && Lex.Get().Type == TK_EQUAL) {
			throw UnexpectedSymbol(";", "=", Lex.Get().Pos);
		}
		Expr* Exp = nullptr;
		if (Lex.Get().Type == TK_EQUAL) {
			auto Pos = Lex.Get().Pos;
			Exp = ParseInit(Table);
			CheckType(Table, Type, Exp, Pos);
		}
		for (int i = 0; i < Names.size(); ++i) {
			Table->CheckSymbol(Names[i], Names_Pos[i]);
			Table->Add(new SymVar(Names[i], Exp, Type, State));
		}
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseTypeDecl(SymTable* Table) {
	Lex.NextAndCheck(TK_IDENTIFIER);
	while (Lex.Get().Type == TK_IDENTIFIER) {
		string NameNew = Lex.Get().Source;
		Position NameNew_Pos = Lex.Get().Pos;
		Lex.NextAndCheck(TK_EQUAL);
		Lex.Next();
		Table->CheckSymbol(NameNew, NameNew_Pos);
		Table->Add(new SymType(NameNew, ParseType(Table)));
		Lex.CheckAndNext(TK_SEMICOLON);
	}
}

void Parser::ParseCallDecl(SymTable* Table, DeclSection Section) {
	Lex.NextAndCheck(TK_IDENTIFIER);
	auto Pos = Lex.Get().Pos;
	auto Name = Lex.Get().Source;
	Lex.Next();
	auto LocTable = new SymTable(Table);
	int argc = 0;
	if (Lex.Get().Type == TK_OPEN_BRACKET) {
		Lex.Next();
		argc = ParseArguments(LocTable);
		Lex.CheckAndNext(TK_CLOSE_BRACKET);
	}
	Symbol* NewSym = nullptr;
	if (Section == DeclFunction) {
		Lex.CheckAndNext(TK_COLON);
		auto Type = ParseType(Table);
		LocTable->Add(new SymVar("Result", nullptr, Type, RValue));
		Lex.CheckAndNext(TK_SEMICOLON);
		NewSym = new SymFunction(Name, LocTable, nullptr, argc + 1, Type);
	}
	if (Section == DeclProcedure) {
		Lex.CheckAndNext(TK_SEMICOLON);
		NewSym = new SymProcedure(Name, LocTable, nullptr, argc);
	}
	auto Symbols = Table->GetAllSymbols(Name, Pos);
	SymCall* FirstDeclSym = nullptr;
	for (int i = 0; i < Symbols.size(); ++i) {
		if (CmpArguments().Compare(NewSym, Symbols[i])) {
			if (Symbols[i]->Section == Section && ((SymCall*)Symbols[i])->Stmt == nullptr) {
				FirstDeclSym = (SymCall*)Symbols[i];
			}
			else {
				throw DuplicateIdentifier(Name, Pos);
			}
		}
	}
	if (Lex.Get().Type == TK_FORWARD) {
		if (FirstDeclSym != nullptr) {
			throw DuplicateIdentifier(Name, Pos);
		}
		Lex.Next();
		Lex.CheckAndNext(TK_SEMICOLON);
		DeclForwardCall.push_back({ NewSym, Pos });
		Table->Add(NewSym);
		return;
	}
	ParseDeclSection(LocTable);
	Lex.Check(TK_BEGIN);
	if (FirstDeclSym != nullptr) {
		FirstDeclSym->Stmt = ParseStatement(LocTable, 0);
	}
	else {
		((SymCall*)NewSym)->Stmt = ParseStatement(LocTable, 0);
		Table->Add(NewSym);
	}
}

Symbol* Parser::ParseRecord(SymTable* Table) {
	Lex.Next();
	auto LocTable = new SymTable(Table);
	int argc = ParseArguments(LocTable);
	for (int i = 0, offset = 0; i < argc; ++i) {
		((SymIdent*)LocTable->Symbols[i])->offset = offset;
		offset += ((SymIdent*)LocTable->Symbols[i])->Type->GetSize();
	}
	Lex.CheckAndNext(TK_END);
	return new SymRecord(LocTable, "", argc);
}

int Parser::ParseArguments(SymTable* Table) {
	int argc = 0;
	while (Lex.Get().Type == TK_IDENTIFIER || Lex.Get().Type == TK_CONST || Lex.Get().Type == TK_VAR || Lex.Get().Type == TK_OUT) {
		ArgState State = RValue;
		if (Lex.Get().Type == TK_CONST) {
			State = Const;
			Lex.Next();
		}
		if (Lex.Get().Type == TK_VAR) {
			State = Var;
			Lex.Next();
		}
		if (Lex.Get().Type == TK_OUT) {
			//State = Out;
			State = Var;
			Lex.Next();
		}
		++argc;
		auto Pos = Lex.Get().Pos;
		auto Name = Lex.Get().Source;
		Lex.Next();
		Lex.CheckAndNext(TK_COLON);
		auto Type = ParseType(Table);
		Table->CheckSymbol(Name, Pos);
		Table->Add(new SymVar(Name, nullptr, Type, State));
		if (Lex.Get().Type != TK_END  && Lex.Get().Type != TK_CLOSE_BRACKET) {
			Lex.CheckAndNext(TK_SEMICOLON);
		}
	}
	return argc;
}

Symbol* Parser::ParseArray(SymTable* Table){
	Lex.Next();
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Expr* Exp_Left = ParseExpr(Table); 
		CheckConstExpr(Table, Exp_Left);
		int A = CalculateConstExpr<int>(Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Left);
		Lex.CheckAndNext(TK_DOUBLE_POINT);
		Expr* Exp_Right = ParseExpr(Table);
		CheckConstExpr(Table, Exp_Right);
		int B = CalculateConstExpr<int>(Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Right);
		SymArray* Sym = new SymArray(nullptr, A, B);
		Symbol** Sym_TypeInit = &Sym->Type;
		while (Lex.Get().Type == TK_COMMA){
			Lex.Next();
			Exp_Left = ParseExpr(Table);
			CheckConstExpr(Table, Exp_Left);
			A = CalculateConstExpr<int>(Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Left);
			Lex.CheckAndNext(TK_DOUBLE_POINT);
			Exp_Right = ParseExpr(Table);
			CheckConstExpr(Table, Exp_Right);
			B = CalculateConstExpr<int>(Table, "INTEGER", Lex.Get().Pos).Calculate(Exp_Right);
			*Sym_TypeInit = new SymArray(nullptr, A, B);
			Sym_TypeInit = &((SymArray*)*Sym_TypeInit)->Type;
		}
		Lex.CheckAndNext(TK_CLOSE_SQUARE_BRACKET);
		Lex.CheckAndNext(TK_OF);
		auto Pos = Lex.Get().Pos;
		*Sym_TypeInit = ParseType(Table);
		while (((SymType*)*Sym_TypeInit)->TypeID == TypeID_BadType) {
			auto Sym = Table->GetSymbol((*Sym_TypeInit)->Name, Pos);
			*Sym_TypeInit = Sym->GetType();
		}
		return Sym;
	}
	Lex.CheckAndNext(TK_OF);
	return new SymDynArray(ParseType(Table));
}

Symbol* Parser::ParseString(SymTable* Table){
	Lex.Next();
	int Length = -1;
	if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
		Lex.Next();
		Position Pos = Lex.Get().Pos;
		auto Exp_Length = ParseExpr(Table);
		CheckConstExpr(Table, Exp_Length);
		Length = CalculateConstExpr<int>(Table, "INTEGER", Pos).Calculate(Exp_Length);
		Lex.Check(TK_CLOSE_SQUARE_BRACKET);
		Lex.Next();
	}
	return new SymStringType(Length);
}

Expr* Parser::ParseInitList(SymTable* Table){
	ExprInitList* Ans = new ExprInitList();
	do {
		Lex.Next();
		if (Lex.Get().Type == TK_OPEN_BRACKET) {
			Ans->List.push_back(ParseInitList(Table));
			Lex.CheckAndNext(TK_CLOSE_BRACKET);
			if (Lex.Get().Type != TK_COMMA) {
				return Ans;
			}
			continue;
		}
		auto Exp = ParseExpr(Table);
		CheckConstExpr(Table, Exp);
		Ans->List.push_back(Exp);
	} while (Lex.Get().Type == TK_COMMA);
	return Ans;
}

Expr* Parser::ParseInit(SymTable* Table){
	Lex.Next();
	int count = 0;
	if (Lex.Get().Type == TK_OPEN_BRACKET){
		auto Exp = ParseInitList(Table);
		Lex.CheckAndNext(TK_CLOSE_BRACKET);
		return Exp;
	}
	auto Exp = ParseExpr(Table);
	CheckConstExpr(Table, Exp);
	return Exp;
}

Symbol* Parser::ParseType(SymTable* Table){
	switch (Lex.Get().Type){
	case TK_STRING:
		return ParseString(Table);
	case TK_ARRAY:
		return ParseArray(Table);
	case TK_RECORD:
		return ParseRecord(Table);
	default:
		bool flag = false;
		if (Lex.Get().Type == TK_CAP) {
			flag = true;
			Lex.Next();
		}
		auto Pos = Lex.Get().Pos;
		if (!Table->Find(Lex.Get().Source)) {
			throw UnknownType(Lex.Get().Source, Lex.Get().Pos);
		}
		auto Sym = Table->GetSymbol(Lex.Get().Source, Lex.Get().Pos);
		while (((SymType*)Sym)->TypeID == TypeID_BadType) { 
			auto _Sym = Table->GetSymbol(Sym->Name, Pos);
			Sym = _Sym->GetType();
		}
		Lex.Next();
		if (flag) {
			return new SymPointer("pointer", Sym);
		}
		return Sym;
	}
}

void Parser::CheckConstExpr(SymTable* Table, Expr* Exp) {
	auto List = new ExpArgList();
	Exp->GetIdentStr(List);
	if (List->Flag == false) {
		throw ExpectedConstExp(Lex.Get().Pos);
	}
	for (int i = 0; i < List->Vec.size(); ++i) {
		if (Table->Find(List->Vec[i]) != -1 && Table->GetSymbol(List->Vec[i], Lex.Get().Pos)->Section != DeclConst) {
			throw ExpectedConstExp(Lex.Get().Pos);
		}
	}
}

/* Parse Expr */

Expr* Parser::ParseExpr(SymTable* Table){
	auto Left = ParseLevel(Table, 0);
	if (Lex.Get().Type == TK_CAP) {
		Left = new ExprDereference(Left);
		Lex.Next();
	}
	auto TK = Lex.Get();
	if (TK.Type == TK_ASSIGNED){
		Lex.Next();
		auto Right = ParseLevel(Table, 0);
		if (Left_Op_Assign.find(Left->TypeExp) == Left_Op_Assign.cend()){
			throw ExpectedVariable(Lex.Get().Pos);
		}
		if (Lex.Get().Type == TK_CAP) {
			Right = new ExprDereference(Right);
			Lex.Next();
		}
		return new ExprAssign(Left, Right);
	}
	return Left;
}

Expr* Parser::ParseLevel(SymTable* Table, const int level){
	if (level == parse_factor_level){
		return ParseFactor(Table);
	}
	auto Left = ParseLevel(Table, level + 1);
	while (level_list[level].find(Lex.Get().Type) != level_list[level].cend()){
		Token Op = Lex.Get();
		Lex.Next();
		Left = new ExprBinOp(Left, Op, ParseLevel(Table, level + 1));
	}
	if (Lex.Get().Type == TK_CAP) {
		Left = new ExprDereference(Left);
		Lex.Next();
	}
	return Left;
}

Expr* Parser::ParseFactor(SymTable* Table){
	auto TK = Lex.Get();
	if (TK.Type == NOT_TOKEN){
		throw IllegalExpr(TK.Pos);
	}
	if (TK.Type == TK_IDENTIFIER){
		return ParseDesignator(Table);
	}
	Lex.Next();
	if (TK.Type == TK_MINUS || TK.Type == TK_PLUS){
		return new ExprUnarOp(TK, ParseLevel(Table, 0));
	}
	if (TK.Type == TK_OPEN_BRACKET){
		auto ExpNow = ParseLevel(Table, 0);
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
		return new ExprUnarOp(TK, ParseLevel(Table, 0));
	}
	if (TK.Type == TK_DOG) {
		auto Exp = ParseLevel(Table, 0);
		if (Left_Op_Assign.find(Exp->TypeExp) == Left_Op_Assign.cend()) {
			throw ExpectedVariable(TK.Pos);
		}
		return new ExprPointer(Exp);
	}
	throw IllegalExpr(TK.Pos);
}

Expr* Parser::ParseDesignator(SymTable* Table){
	auto TK = Lex.Get();
	Lex.Next();
	if (State == Test_Exp) {
		Table->Add(new SymVar(TK.Source, nullptr, nullptr, RValue));
	}
	auto Sym = Table->GetSymbol(TK.Source, TK.Pos);
	Expr* ExpNow = new ExprIdent(Sym, TK.Pos);
	while (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET || Lex.Get().Type == TK_OPEN_BRACKET || Lex.Get().Type == TK_POINT){
		if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
			do {
				Lex.Next();
				ExpNow = new ExprArrayIndex(ExpNow, ParseLevel(Table, 0));
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
				Arguments.push_back(ParseLevel(Table, 0));
				while (Lex.Get().Type == TK_COMMA){
					Lex.Next();
					Arguments.push_back(ParseLevel(Table, 0));
				} 
			}
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw UnexpectedSymbol(")", Lex.Get().Source, Lex.Get().Pos);
			}
			Lex.Next();
			ExpNow = new ExprFunction(ExpNow, Arguments);
			if (State != Test_Exp) {
				auto Sym = Table->FindRequiredSymbol(ExpNow, TK.Pos);
				((ExprIdent*)((ExprFunction*)ExpNow)->Left)->Sym = Sym;
			}
		}
		if (Lex.Get().Type == TK_POINT){
			Lex.Next();
			if (Lex.Get().Type != TK_IDENTIFIER){
				throw UnexpectedSymbol("identifier", Lex.Get().Source, Lex.Get().Pos);
			}
			Symbol* Right = nullptr;
			if (State == Test_Exp) {
				Right = new SymVar(Lex.Get().Source, nullptr, nullptr, RValue);
			}
			else {
				if (Sym->Section == DeclFunction) {
					Right = ((SymRecord*)Sym->GetType())->Table->GetSymbol(Lex.Get().Source, Lex.Get().Pos);
				}
				else {
					Right = ((SymRecord*)Sym->GetType())->Table->GetSymbol(Lex.Get().Source, Lex.Get().Pos);
				}
			}
			Lex.Next();
			Sym = Right;
			ExpNow = new ExprRecord(ExpNow, Right);
		}
	}
	if (ExpNow->TypeExp == VarExp && Sym->Section == DeclProcedure) {
		ExpNow = new ExprFunction(ExpNow, vector<Expr*>());
	}
	return ExpNow;
}