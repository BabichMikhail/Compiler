#ifndef LLEX_H
#define LLEX_H

#include <stdio.h>
#include <string>
#include <iostream>
#include "errors.h"

enum TokenType{
	TK_AND, TK_ARRAY, TK_BEGIN, TK_BOOLEAN, TK_BREAK, 
	TK_CASE, TK_CHAR, TK_CONST,
	TK_DIV_INT, TK_DOWNTO, TK_DOUBLE, TK_DO, 
	TK_EXCEPT, TK_ELSE, TK_END, TK_EXIT, 
	TK_FALSE, TK_FINALLY, TK_FOR, TK_FUNCTION, 
	TK_GOTO, TK_IF, TK_INTEGER, 
	TK_LABEL, TK_MOD, TK_NIL, TK_NOT, 
	TK_OF, TK_OR, TK_PROCEDURE, TK_PROGRAM, 
	TK_RECORD, TK_REAL, TK_REPEAT, 
	TK_SHL, TK_SHR, TK_STRING, 
	TK_THEN, TK_TO, TK_TRUE, TK_TRY, TK_TYPE, 
	TK_UNTIL, TK_VAR, TK_WHILE, TK_XOR,

	TK_NOT_EQUAL, TK_LESS_EQUAL, TK_GREAT_EQUAL, TK_ASSIGNED, 
	TK_PLUS_EQUAL, TK_SUB_EQUAL, TK_MUL_EQUAL, TK_DIV_EQUAL, 

	TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_EQUAL, 
	TK_LESS, TK_GREAT, TK_OPEN_SQUARE_BRACKET, TK_CLOSE_SQUARE_BRACKET, TK_DOUBLE_POINT,
	TK_POINT, TK_COMMA, TK_OPEN_BRACKET, TK_CLOSE_BRACKET, TK_COLON, 
	TK_CAP,	TK_DOG, TK_OPEN_BRACE, TK_CLOSE_BRACE, TK_DOLLAR, 
	TK_GRILL, TK_AMPERSAND, TK_PERCENT, TK_SEMICOLON,

	TK_IDENTIFIER, TK_REAL_VALUE, TK_INTEGER_VALUE, TK_STRING_VALUE, TK_HEX_VALUE,
	NOT_TOKEN
}; 

class Token{
private:
	Position Pos;
	void Set(const Position Pos, const std::string Source, const TokenType Type);
public:
	friend class Lexer;
	TokenType Type;
	std::string Source;

	Token() : Pos(0, 0), Source(), Type(NOT_TOKEN){};
	Token(const Token &TK) : Pos(TK.Pos), Source(TK.Source), Type(TK.Type){};
	Token(const Position Pos, const string S, const TokenType T) : Pos(Pos), Source(S), Type(T){};
};

class Lexer{
private:
	Token TK;
	FILE *f_in;
	string String;
	string::iterator It;
	Position Pos;
	int Column_Offset; 
	bool IsTokens;

	bool CanNumberLexem(const string::iterator It);
	bool CanReservedWords(const string::iterator It);

	void SetToken(const Position Pos, const string Source, const int Length, const TokenType Type);

	void SetNumberNotHex(const Position Pos);
	void SetNumberHex(const Position Pos);
	void SetComment(const Position Pos);
	void SetString(const Position Pos);

	void FindNext();
	void NewString();
public:
	Lexer(const char* filename);
	Lexer(FILE *f);
	
	const bool isToken();
	const Token Get();
	void Next();

	void AssertAndNext(const TokenType TT);
	void Assert(const TokenType TT);
	void NextAndAssert(const TokenType TT);
	
	void Print();
};

#endif