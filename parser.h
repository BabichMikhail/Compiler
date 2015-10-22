#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "symtable.h"
#include "lexer.h" 
#include <vector>
#include <set>

using namespace std;

enum PState { Test_Exp, Test_Def };

class Parser{
private:
	Lexer Lex;
	SymTable Table;
	Expr* Exp;
public:
	Parser(const char* filename, PState State);
	Expr* ParseLevel(const int level);
	Expr* ParseFactor();
	Expr* ParseExpr();
	Expr* ParseDesignator();

	void ParseLabel();
	void ParseConst();
	void ParseVar();
	void ParseType();
	//void ParseFunction();
	//void ParseProcedure();

	Symbol* ParseArray(SymState State = State_NULL, string Name = "");
	template <class ArrayType> Symbol* ParseArrayOF(SymState State = State_NULL, string Name = "", Expr* Exp_Left = nullptr, Expr* Exp_Right = nullptr);
	Symbol* ParseString(SymState State = State_NULL, string Name = "");
	template <class Sym_X_Type> Symbol* ParseSimpleType(SymState State  = State_NULL, string Name = "");
	Symbol* ParseIdentifier(SymState State = State_NULL, string Name = "", string TypeName = "");

	vector<Expr*> ParseEqual();
	void AssertConstExpr(Expr* Exp);

	void Print();
};

#endif