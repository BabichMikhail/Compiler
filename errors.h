#ifndef ERRORS_H
#define ERRORS_H

typedef struct Position{
	Position(const int L = 0, const int C = 0) : Line(L), Column(C){};
	Position(const Position& Pos) : Line(Pos.Line), Column(Pos.Column){};
	int Line, Column;
} Position;

#include <string>

using namespace std;

static string CheckSymbol(const string Str){
	if (Str.length() == 1){
		if (Str[0] < 0 || Str[0] > 127){
			return "EOF";
		}
	}
	return Str;
}

static string StrPos(const Position Pos) {
	return "Line " + to_string(Pos.Line + 1) + " Column " + to_string(Pos.Column + 1);
}

class Error{
public:
	Position Pos;
	string err_msg;
	Error(const string err_msg, const Position Pos): err_msg(err_msg){}
};

class BadType : public Error {
public:
	BadType(const string TypeExpected, const Position Pos) : Error("Error: Incompatible types: expected \"" + TypeExpected + "\"" + StrPos(Pos), Pos) {};
};

class BadChar: public Error{
public: 
	BadChar(const Position Pos) : 
		Error("Unknown Symbol in " + StrPos(Pos), Pos){}
};

class BadExp: public Error{
public: 
	BadExp(const Position Pos) : 
		Error("Incorrect Float Number in " + StrPos(Pos) + ". There is no +/- after e", Pos){}
};

class NoFract: public Error{
public: 
	NoFract(const Position Pos) : 
		Error("Incorrect Float Number in " + StrPos(Pos) + ". There is no number after point", Pos){}
};

class BadCC: public Error{
public: 
	BadCC(const Position Pos) : Error("Incorrect Symbol in " + StrPos(Pos), Pos){}
};

class BadNL: public Error{
public: 
	BadNL(const Position Pos) : Error("Incorrect Symbol in " + StrPos(Pos), Pos){}
};

class BadEOF: public Error{
public: 
	BadEOF(const Position Pos) : Error("Unexpected EOF in " + StrPos(Pos), Pos){};
};

class UnexpectedSymbol: public Error{
public: 
	UnexpectedSymbol(const string Symbol, const string Str_Found, const Position Pos) : 
		Error("Fatal: Syntax error, \"" + Symbol + "\" expected but \"" + CheckSymbol(Str_Found) + "\" found in " + StrPos(Pos), Pos){}
};

class IllegalExpr: public Error{
public:	
	IllegalExpr(const Position Pos) : Error("Error: Illegal Expression. " + StrPos(Pos), Pos){}
};

class ExpectedVariable : public Error{
public:
	ExpectedVariable(const Position Pos) : Error("Error: Variable identifier expected. " + StrPos(Pos), Pos){}
};

class ExpectedConstExp : public Error{
public:
	ExpectedConstExp(const Position Pos) : Error("Error: Const Expression expected. " + StrPos(Pos), Pos){}
};

class UnknownType : public Error{
public:
	UnknownType(string TypeName, Position Pos) : Error("Error: Unknown TypeName \"" + CheckSymbol(TypeName) + "\". " + StrPos(Pos), Pos){};
};

class NotAllowedStmt : public Error {
public:
	NotAllowedStmt(const string StmtName, const Position Pos) : Error("Error: " + StmtName + " not allowed. " + StrPos(Pos), Pos) {};
};

class IdentifierNotFound : public Error {
public:
	IdentifierNotFound(const string IdName, const Position Pos) : Error("Error: Identifier \"" + IdName + "\" not found. " + StrPos(Pos), Pos) {};
};

class DuplicateIdentifier : public Error {
public:
	DuplicateIdentifier(const string IdName, const Position Pos) : Error("Error: Duplicate Identifier \"" + IdName + "\". " + StrPos(Pos), Pos) {};
};

class IncompatibleTypes : public Error {
public:
	IncompatibleTypes(const string Type_Expected, const string Type_Found, const Position Pos):
		Error("Error: Incompatible Types. Expected \"" + Type_Expected + "\" but \"" + Type_Found + "\" found. " + StrPos(Pos), Pos) {};
};

#endif 