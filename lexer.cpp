#define _CRT_SECURE_NO_WARNINGS

#include "lexer.h"
#include "errors.h"

using namespace std;

static const char *const Tokens_str[] = {
	"and", "array", "begin", "boolean", "break", 
	"case", "const", "continue",
	"div", "downto", "do", 
	"except", "else", "end", "exit", 
	"false", "finally", "forward", "for", "function",
	"goto", "if",
	"label", "mod", "nil", "not", 
	"of", "or", "out", "procedure", "program", 
	"raise", "record", "repeat",
	"shl", "shr", "string", 
	"then", "to", "true", "try", "type", 
	"until", "var", "while", "xor",

	"<>", "<=", ">=", ":=", 
	"+=", "-=", "*=", "/=",

	"+", "-", "*", "/", "=",
	"<", ">", "[", "]", "..", 
	".", ",", "(", ")", ":", 
	"^", "@", "{", "}", "$", 
	"#", "&", "%", ";", 
	
	"identifier", "double_value", "integer_value", "string_value", "hex_value"
};

TokenType Reserved_Words[] = {
	TK_AND, TK_ARRAY, TK_BEGIN, TK_BOOLEAN, TK_BREAK, TK_CASE, TK_CONST, TK_CONTINUE, TK_DIV_INT, TK_DOWNTO, TK_DO, TK_EXCEPT, TK_ELSE, 
	TK_END, TK_EXIT, TK_FALSE, TK_FINALLY, TK_FOR, TK_FORWARD, TK_FUNCTION, TK_GOTO, TK_IF, TK_LABEL, TK_MOD, TK_NIL, TK_NOT, TK_OF, TK_OR, TK_OUT,
	TK_PROCEDURE, TK_PROGRAM, TK_RAISE, TK_RECORD, TK_REPEAT, TK_SHL, TK_SHR, TK_STRING, TK_THEN, TK_TO, TK_TRUE, TK_TRY, TK_TYPE, TK_UNTIL, TK_VAR, TK_WHILE, 
	TK_XOR
};

TokenType Operators[] = {
	TK_NOT_EQUAL, TK_LESS_EQUAL, TK_GREAT_EQUAL, TK_ASSIGNED, TK_PLUS_EQUAL, TK_SUB_EQUAL, TK_MUL_EQUAL, TK_DIV_EQUAL, TK_PLUS, TK_MINUS, TK_MUL, 
	TK_DIV, TK_EQUAL, TK_LESS, TK_GREAT, TK_OPEN_SQUARE_BRACKET, TK_CLOSE_SQUARE_BRACKET, TK_DOUBLE_POINT, TK_POINT, TK_COMMA, TK_OPEN_BRACKET, 
	TK_CLOSE_BRACKET, TK_COLON, TK_CAP, TK_DOG, TK_OPEN_BRACE, TK_CLOSE_BRACE, TK_DOLLAR, TK_GRILL, TK_AMPERSAND, TK_PERCENT, TK_SEMICOLON
};

void Token::Set(const Position Pos, const string Source, const TokenType Type){
	this->Pos.Line = Pos.Line;
	this->Pos.Column = Pos.Column;
	this->Source = Source;
	this->Type = Type;
}

const bool Lexer::isToken(){
	return IsTokens;
}

const Token Lexer::Get(){
	return Token(TK);
}

void Lexer::Check(const TokenType TT){
	if (TK.Type != TT){
		throw UnexpectedSymbol(Tokens_str[TT], TK.Source, TK.Pos);
	}
}

void Lexer::CheckAndNext(const TokenType TT){
	Check(TT);
	Next();
}

void Lexer::NextAndCheck(const TokenType TT){
	Next();
	Check(TT);
}

bool Lexer::CanNumberLexem(const string::iterator It){
	char c = *It;
	return !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

bool Lexer::CanReservedWords(const string::iterator It){
	char c = *It;
	return !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'));
}

void Lexer::SetNumberNotHex(const Position Pos){
	string::iterator It_Count = It + 1;
	while (*It_Count >= '0' && *It_Count <= '9')
		++It_Count;
	if ((*It_Count == '.' || *It_Count == 'e') && *(It_Count + 1) != '.'){ /* Float */
		if (*It_Count == '.'){ /* A.B */
			++It_Count;
			if (*It_Count < '0' || *It_Count > '9'){
				throw NoFract(Pos);
			}
		}
		else { /* Ae+/-B*/
			++It_Count;
			if (*It_Count != '+' && *It_Count != '-'){
				throw BadExp(Pos);
			}
			++It_Count;
		}
		do	++It_Count; while (*It_Count >= '0' && *It_Count <= '9');
		if (!CanNumberLexem(It_Count)){
			throw BadChar(Pos);
		}
		SetToken(Pos, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_REAL_VALUE);
	} /* Integer */
	else{
		if (!CanNumberLexem(It_Count)){
			throw BadChar(Pos);
		}
		SetToken(Pos, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_INTEGER_VALUE);
	}
}

void Lexer::SetNumberHex(const Position Pos){
	string::iterator It_Count = It + 1;
	while ((*It_Count >= '0' && *It_Count <= '9') || (*It_Count >= 'A' && *It_Count <= 'F')){
		++It_Count;
	}
	if (It_Count - It == 1 || !CanNumberLexem(It_Count)){
		throw BadChar(Pos);
	}
	SetToken(Pos, String.substr(It - String.cbegin() + 1, It_Count - It - 1), It_Count - It, TK_HEX_VALUE);
}

void Lexer::SetComment(const Position Pos){
	string::iterator It_Count = It + 1;
	int Open_Brace_Count = 1;
	while (Open_Brace_Count > 0){
		if (*It_Count == EOF){
			throw BadEOF(Pos.Line);
		}
		if (*It_Count == '}'){
			--Open_Brace_Count;
		}
		if (*It_Count == '{'){
			++Open_Brace_Count;
		}
		if (*It_Count == '\n'){
			NewString();
			It_Count = String.begin();
			continue;
		}
		++It_Count;
	}
	SetToken(Pos, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, NOT_TOKEN);
}

void Lexer::SetString(const Position Pos){
	string::iterator It_Count = It + 1;
	while (*It_Count != '\'' && *It_Count != '\n' && *It_Count != EOF){
		++It_Count;
	}
	switch (*It_Count){
	case '\'':
		SetToken(Pos, String.substr(It - String.cbegin() + 1, It_Count - It - 1), It_Count - It + 1, TK_STRING_VALUE);
		break;
	case '\n': throw BadNL(Pos);
	case EOF: throw BadEOF(Pos.Line); 
	}
}

void Lexer::SetToken(const Position Pos, const string Source, const int Length, const TokenType Type){
	TK.Set(Pos, Source, Type);
	It += Length;
}

void Lexer::Next(){
	do FindNext(); while (IsTokens && TK.Type == NOT_TOKEN);
}

void Lexer::FindNext(){
	Pos.Column = It - String.cbegin() + Column_Offset;
	if ((*It < 0 || *It > 127) && *It != EOF){
		throw BadCC(Pos);
	}
	while (*It == ' ' || *It == '\t'){  /* Find Next Possible Token */
		if (*It == ' '){
			++Pos.Column;
		}
		else if (*It == '\t'){
			Pos.Column += 4;
			Column_Offset += 3;
		}
		++It;
	}

	if (*It >= '0' && *It <= '9'){
		SetNumberNotHex(Pos); /* set float or integer value */
		return;
	}
	else if (*It == '$'){
		SetNumberHex(Pos); /* set hex value */
		return;
	}
	else if (*It == '{'){
		SetComment(Pos); /* cut comment */
		return;
	}
	else if (String.cend() - It > 2 && *It == '/' && *(It + 1) == '/'){
		SetToken(Pos, "", 0, NOT_TOKEN); /* cut comment */
		this->NewString();
		return;
	}
	else if (*It == '\''){
		SetString(Pos); /* set string value */
		return;
	}
	else if (*It == '\n'){
		SetToken(Pos, "", 0, NOT_TOKEN); /* end string */
		this->NewString();
		return;
	}
	else if (*It == EOF){
		SetToken(Pos, String.substr(It - String.cbegin(), 1), 1, NOT_TOKEN); /* end file */
		IsTokens = false;
		return;
	}

	for (int i = 0; i < sizeof(Reserved_Words) / sizeof(TokenType); ++i){ /* Other Tokens from array 'Special_Symbols' */
		int len = strlen(Tokens_str[Reserved_Words[i]]);
		if (_strnicmp(String.substr(It - String.cbegin(), len).c_str(), Tokens_str[Reserved_Words[i]], len) == 0 && CanReservedWords(It + len)){
			SetToken(Pos, String.substr(It - String.cbegin(), len), len, Reserved_Words[i]);
			return;
		}
	}

	for (int i = 0; i < sizeof(Operators) / sizeof(TokenType); ++i){ /* Other Tokens from array 'Operators' */
		int len = strlen(Tokens_str[Operators[i]]);
		if (String.compare(It - String.cbegin(), len, Tokens_str[Operators[i]]) == 0){
			SetToken(Pos, String.substr(It - String.cbegin(), len), len, Operators[i]);
			return;
		}
	}

	string::iterator It_Count = It + 1; /* set identifier or error */
	if (!(*It >= 'a' && *It <= 'z' || *It >= 'A' && *It <= 'Z' || *It >= '0' && *It <= '9' || *It == '_')){
		throw BadNL(Pos);
	}
	while (*It_Count >= 'a' && *It_Count <= 'z' || *It_Count >= 'A' && *It_Count <= 'Z' || *It_Count >= '0' && *It_Count <= '9' || *It_Count == '_'){
		++It_Count;
	}
	SetToken(Pos, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_IDENTIFIER);
}

void Lexer::Print(){
	if (TK.Type != NOT_TOKEN)
		cout << TK.Pos.Line + 1 << "\t" << TK.Pos.Column + 1 << "\t" << Tokens_str[TK.Type] << "\t" << TK.Source.c_str() << endl;
}

void Lexer::NewString(){
	if (String.length() > 0 && *String.crbegin() == EOF){
		IsTokens = false;
		return;
	}
	string NewStr;
	do 	NewStr += getc(f_in); while (*NewStr.crbegin() != EOF && *NewStr.crbegin() != '\n');
	String.assign(NewStr);
	this->It = String.begin();
	++Pos.Line;
	Pos.Column = 0;
	Column_Offset = 0;
}

Lexer::Lexer(const char* filename) : TK(), f_in(fopen(filename, "r")), IsTokens(true), Pos(-1, -1){
	NewString();
}

Lexer::Lexer(FILE *f) : TK(),  f_in(f), IsTokens(true), Pos(-1, -1){
	NewString();
}