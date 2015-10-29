#ifndef ERRORS_H
#define ERRORS_H

typedef struct Position{
	Position(const int L = 0, const int C = 0) : Line(L), Column(C){};
	Position(const Position& Pos) : Line(Pos.Line), Column(Pos.Column){};
	int Line, Column;
} Position;

#include <string>

using namespace std;

static string CheckSymbol(string Str){
	if (Str.length() == 1){
		if (Str[0] < 0 || Str[0] > 127){
			return "EOF";
		}
	}
	return Str;
}

class Error{
public:
	string err_msg;
	Error(string err_msg): err_msg(err_msg){}
};

class BadType : public Error {
public:
	BadType(string TypeExpected) : Error("Error: Incompatible types: expected \"" + TypeExpected + "\"") {};
};

class BadChar: public Error{
public: 
	BadChar(const Position Pos) : Error("Unknown Symbol in Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1)){}
};

class BadExp: public Error{
public: 
	BadExp(const Position Pos) : Error("Incorrect Float Number in Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1) + ". There is no +/- after e"){}
};

class NoFract: public Error{
public: 
	NoFract(const Position Pos) : Error("Incorrect Float Number in Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1) + ". There is no number after point"){}
};

class BadCC: public Error{
public: 
	BadCC(const Position Pos) : Error("Incorrect Symbol in Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1)){}
};

class BadNL: public Error{
public: 
	BadNL(const Position Pos) : Error("Incorrect Symbol in Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1)){}
};

class BadEOF: public Error{
public: 
	BadEOF(int Line) : Error("Unexpected EOF in Line " + to_string(Line + 1)){};
};

class UnexpectedSymbol: public Error{
public: 
	UnexpectedSymbol(string Symbol, string Str_Found) : Error("Fatal: Syntax error, \"" + Symbol + "\" expected but \"" + CheckSymbol(Str_Found) + "\" found"){}
};

class IllegalExpr: public Error{
public:	
	IllegalExpr() : Error("Error: Illegal Expression"){}
};

class ExpectedVariable : public Error{
public:
	ExpectedVariable() : Error("Error: Variable identifier expected"){}
};

class ExpectedConstExp : public Error{
public:
	ExpectedConstExp() : Error("Error: Const Expression expected"){}
};

class UnknownType : public Error{
public:
	UnknownType(string TypeName) : Error("Error: Unknown TypeName \"" + CheckSymbol(TypeName) + "\""){};
};

class NotAllowedStmt : public Error {
public:
	NotAllowedStmt(string StmtName) : Error("Error: " + StmtName + " not allowed") {};
};

#endif 