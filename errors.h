#ifndef ERRORS_H
#define ERRORS_H

#include <string>

using namespace std;

class Error{
public:
	std::string err_msg;
	Error(std::string err_msg): err_msg(err_msg){}
};

class BadChar: public Error{
public: 
	BadChar(int Line, int Column) : Error("Unknown Symbol in Line " + to_string(Line) + ", Column " + to_string(Column)){}
};

class BadExp: public Error{
public: 
	BadExp(int Line, int Column) : Error("Incorrect Float Number in Line " + to_string(Line) + ", Column " + to_string(Column) + ". There is no +/- after e"){}
};

class NoFract: public Error{
public: 
	NoFract(int Line, int Column) : Error("Incorrect Float Number in Line " + to_string(Line) + " Column + " + to_string(Column) + ". There is no number after point"){}
};

class BadCC: public Error{
public: 
	BadCC(int Line, int Column) : Error("Incorrect Symbol in Line " + to_string(Line) + " Column " + to_string(Column)){}
};

class BadNL: public Error{
public: 
	BadNL(int Line, int Column) : Error("Incorrect Symbol in Line " + to_string(Line) + " Column " + to_string(Column)){}
};

class BadEOF: public Error{
public: 
	BadEOF(int Line) : Error("Unexpected end of file in Line " + to_string(Line)){};
};

class AbsentBrackect: public Error{
public: 
	AbsentBrackect(std::string S) : Error("Fatal: Syntax error, ')' excepted but \"" + S + "\" found"){}
};

class IllegalExpr: public Error{
public:	
	IllegalExpr() : Error("Error: Illegal Expression"){}
};

#endif 