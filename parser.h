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
	SymTable* Table;
	Expr* Exp;
	Statement* Stmt;
	PMod State;
public:
	Parser(const char* filename, PMod State);
	Expr* ParseLevel(const int level);
	Expr* ParseFactor();
	Expr* ParseExpr();
	Expr* ParseDesignator();

	void ParseDeclSection(SymTable* Table);
	void ParseLabelDecl(SymTable* Table);
	void ParseConstDecl(SymTable* Table);
	void ParseVarDecl(SymTable* Table);
	void ParseTypeDecl(SymTable* Table);
	void ParseCallDecl(SymTable* Table, DeclSection Section);
	
	Symbol* ParseRecord(SymTable* Table);
	Symbol* ParseArray(SymTable* Table);
	Symbol* ParseString(SymTable* Table);
	Symbol* ParseType(SymTable* Table);
	int ParseArguments(SymTable* Table);

	Expr* ParseInit(SymTable* Table);
	Expr* ParseInitList(SymTable* Table);
	void CheckConstExpr(SymTable* Table, Expr* Exp);

	Statement* ParseStatement(SymTable* Table, int State);
	Statement* ParseGOTOStmt(SymTable* Table, int State);
	Statement* ParseCompoundStmt(SymTable* Table, int State);
	Statement* ParseIfStmt(SymTable* Table, int State);
	Statement* ParseCase(SymTable* Table, int State);
	Statement* ParseForStmt(SymTable* Table, int State);
	Statement* ParseWhileStmt(SymTable* Table, int State);
	Statement* ParseRepeatStmt(SymTable* Table, int State);
	Statement* ParseTryStmt(SymTable* Table, int State);
	Statement* ParseIdentifier(SymTable* Table, int State);

	void ReservedCastFunction(SymTable* Table, string Type_1, string Type_2);
	void ReservedFunctions(SymTable* Table);

	vector<Statement*> ParseStmtList(SymTable* Table, int State);
	void CheckSemicolon();

	void Print();
};

#endif