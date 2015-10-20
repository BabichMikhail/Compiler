#ifndef PARSER_H
#define PARSER_H

#include "syntaxnodes.h"
#include "lexer.h" 
#include <vector>
#include <set>

using namespace std;

class Parser{
private:
	Lexer Lex;
	Expr* Exp;
public:
	Parser(const char* filename);
	Expr* ParseLevel(const int level);
	Expr* ParseFactor();
	Expr* ParseExpr();
	Expr* ParseDesignator();
	void Print();
};

#endif