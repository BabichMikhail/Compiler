#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "symtable.h"
#include "lexer.h" 
#include <vector>
#include <set>
#include "statement.h"
#include "checktype.h"

using namespace std;

enum PMod { Test_Exp, Test_Decl, Test_Statement };

class Parser{
private:
	Lexer Lex;
	SymTable Table;
	Expr* Exp;
	Statement* Stmt;
	PMod State;
public:
	Parser(const char* filename, PMod State);
	Expr* ParseLevel(const int level);
	Expr* ParseFactor();
	Expr* ParseExpr();
	Expr* ParseDesignator();

	void ParseDeclSection();
	void ParseLabelDecl();
	void ParseConstDecl();
	void ParseVarDecl();
	void ParseTypeDecl();
	//void ParseFunction();
	//void ParseProcedure();

	Symbol* ParseArray();
	Symbol* ParseString();
	Symbol* ParseType();

	Expr* ParseEqual();
	Expr* ParseInitList();
	void CheckConstExpr(Expr* Exp);

	Statement* ParseStatement(int State);
	Statement* ParseGOTOStmt(int State);
	Statement* ParseCompoundStmt(int State);
	Statement* ParseIfStmt(int State);
	Statement* ParseCase(int State);
	Statement* ParseForStmt(int State);
	Statement* ParseWhileStmt(int State);
	Statement* ParseRepeatStmt(int State);
	Statement* ParseTryStmt(int State);

	vector<Statement*> ParseStmtList(int State);
	void CheckSemicolon();

	void Print();
};

#endif