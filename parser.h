#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "symtable.h"
#include "lexer.h" 
#include <vector>
#include <set>

using namespace std;

enum PMod { Test_Exp, Test_Decl };

class Parser{
private:
	Lexer Lex;
	SymTable Table;
	Expr* Exp;
	PMod State;
public:
	Parser(const char* filename, PMod State);
	Expr* ParseLevel(const int level);
	Expr* ParseFactor();
	Expr* ParseExpr();
	Expr* ParseDesignator();

	void ParseLabelDecl();
	void ParseConstDecl();
	void ParseVarDecl();
	void ParseTypeDecl();
	//void ParseFunction();
	//void ParseProcedure();

	Symbol* ParseArray();
	Symbol* ParseString();
	Symbol* ParseType();

	vector<Expr*> ParseEqual();
	void AssertConstExpr(Expr* Exp);
	
	void Print();
};

#endif