#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "symtable.h"
#include "lexer.h" 
#include <vector>
#include <set>

using namespace std;

enum PState { Test_Exp, Test_Decl };

class Parser{
private:
	Lexer Lex;
	SymTable Table;
	Expr* Exp;
	PState State;
public:
	Parser(const char* filename, PState State);
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
	Symbol* ParseIdentifier(string TypeName = "");

	vector<Expr*> ParseEqual();
	void AssertConstExpr(Expr* Exp);
	Symbol* ParseType();

	void Print();
};

#endif