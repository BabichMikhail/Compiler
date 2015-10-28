#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "symtable.h"
#include "lexer.h" 
#include <vector>
#include <set>
#include "statement.h"

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
	void AssertConstExpr(Expr* Exp);

	Statement* ParseStatement();
	Statement* ParseGOTOStmt();
	Statement* ParseCompoundStmt();
	Statement* ParseIfStmt();
	Statement* ParseCase();
	Statement* ParseForStmt();
	Statement* ParseWhileStmt();
	Statement* ParseRepeatStmt();
	Statement* ParseTryStmt();

	void Print();
};

#endif