#define _CRT_SECURE_NO_WARNINGS

#include "lexer.h"
#include "errors.h"

static const char *const Tokens_str[] = {
	"absolute", "and", "array", "as", "begin", 
	"boolean", "bytebool", "byte", "cardinal", "case", 
	"char", "constructor", "const", "comp", "currency", 
	"destructor", "dispose", "div", "downto", "double", 
	"do", "else", "end", "exit", "extended", 
	"false", "file", "for",	"function",	"goto", 
	"if", "implementation", "inherited", "inline", "int64", 
	"interface", "integer", "in", "label", "longbool", 
	"longint", "longword", "mod", "new", "nil", 
	"not", "object", "of", "operator", "or", 
	"packed", "procedure", "program", "record", "reintroduce", 
	"real", "repeat", "self", "set", "shl", 
	"shortint", "shr", "smallint", "single", "string", 
	"text", "then", "to", "true", "type", 
	"qword", "unit", "until", "uses", "var", 
	"wordbool", "word", "while", "with", "xor",

	"<<", ">>", "**", "<>", "<=",
	">=", ":=", "+=", "-=", "*=",
	"/=", "//",

	"+", "-", "*", "/", "=",
	"<", ">", "[", "]", "..", 
	".", ",", "(", ")", ":", 
	"^", "@", "{", "}", "$", 
	"#", "&", "%", ";", 
	
	"identifier", "real_value", "integer_value", "string_value", "hex_value"
};

TokenType Reserved_Words[] = {
	TK_ABSOLUTE, TK_AND, TK_ARRAY, TK_ASM, TK_BEGIN, TK_BOOLEAN, TK_BYTEBOOL, TK_BYTE, TK_CARDINAL, TK_CASE, TK_CHAR, TK_CONSTRUCTOR, TK_CONST, TK_COMP, 
	TK_CURRENCY, TK_DESTRUCTOR, TK_DISPOSE, TK_DIV_INT, TK_DOWNTO, TK_DOUBLE, TK_DO, TK_ELSE, TK_END, TK_EXIT, TK_EXTENDED, TK_FALSE, TK_FILE, TK_FOR,
	TK_FUNCTION, TK_GOTO, TK_IF, TK_IMPLEMENTATION, TK_INHERITED, TK_INLINE, TK_INT64, TK_INTERFACE, TK_INTEGER, TK_IN, TK_LABEL, TK_LONGBOOL, TK_LONGINT,
	TK_LONGWORD, TK_MOD, TK_NEW, TK_NIL, TK_NOT, TK_OBJECT, TK_OF, TK_OPERATOR, TK_OR, TK_PACKED, TK_PROCEDURE, TK_PROGRAM, TK_RECORD, TK_REINTRODUCE,
	TK_REAL, TK_REPEAT, TK_SELF, TK_SET, TK_SHL, TK_SHORTINT, TK_SHR, TK_SMALLINT, TK_SINGLE, TK_STRING, TK_TEXT, TK_THEN, TK_TO, TK_TRUE, TK_TYPE,
	TK_QWORD, TK_UNIT, TK_UNTIL, TK_USES, TK_VAR, TK_WORDBOOL, TK_WORD, TK_WHILE, TK_WITH, TK_XOR,
};

TokenType Operators[] = {
	TK_LESS_LESS, TK_GREAT_GREAT, TK_MUL_MUL, TK_NOT_EQUAL, TK_LESS_EQUAL, TK_GREAT_EQUAL, TK_ASSIGNED, TK_PLUS_EQUAL, TK_SUB_EQUAL, TK_MUL_EQUAL,
	TK_DIV_EQUAL, TK_DIV_DIV, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_EQUAL, TK_LESS, TK_GREAT, TK_OPEN_SQUARE_BRACKET, TK_CLOSE_SQUARE_BRACKET, 
	TK_POINT, TK_COMMA, TK_OPEN_BRACKET, TK_CLOSE_BRACKET, TK_COLON, TK_CAP, TK_DOG, TK_OPEN_BRACE, TK_CLOSE_BRACE, TK_DOLLAR, TK_GRILL, TK_AMPERSAND, 
	TK_PERCENT, TK_SEMICOLON
};

void Token::Set(const int Line, const int Column, const std::string Source, const TokenType Type){
	this->Pos.Line = Line + 1;
	this->Pos.Column = Column + 1;
	this->Source = Source;
	this->Type = Type;
}

bool Lexer::CanNumberLexem(const std::string::iterator It){
	char c = *It;
	return !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

bool Lexer::CanReservedWords(const std::string::iterator It){
	char c = *It;
	return !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'));
}

void Lexer::SetNumberNotHex(const int Line, const int Column){
	std::string::iterator It_Count = It + 1;
	while (*It_Count >= '0' && *It_Count <= '9')
		++It_Count;
	if ((*It_Count == '.' || *It_Count == 'e') && *(It_Count + 1) != '.'){ /* Float */
		
		if (*It_Count == '.'){ /* A.B */
			++It_Count;
			if (*It_Count < '0' || *It_Count > '9'){
				throw NoFract(std::string("Incorrect Float Number in Line %d, Column %d. There is no number after point", Line + 1, Column + 1).c_str());
			}
		}
		else { /* Ae+/-B*/
			++It_Count;
			if (*It_Count != '+' && *It_Count != '-'){
				throw BadExp(std::string("Incorrect Float Number in Line %d, Column %d. There is no +/- after e", Line + 1, Column + 1).c_str());
			}
			++It_Count;
		}
		do	++It_Count; while (*It_Count >= '0' && *It_Count <= '9');
		if (!CanNumberLexem(It_Count)){
			throw BadChar(std::string("Unknown Symbol in Line %d, Column %d", Line + 1, Column + 1).c_str());
		}
		SetToken(Line, Column, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_REAL_VALUE);
	} /* Integer */
	else{
		if (!CanNumberLexem(It_Count)){
			throw BadChar(std::string("Unknown Symbol in Line %d, Column %d", Line + 1, Column + 1).c_str());
		}
		SetToken(Line, Column, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_INTEGER_VALUE);
	}
}

void Lexer::SetNumberHex(const int Line, const int Column){
	std::string::iterator It_Count = It + 1;
	while ((*It_Count >= '0' && *It_Count <= '9') || (*It_Count >= 'A' && *It_Count <= 'F')){
		++It_Count;
	}
	if (It_Count - It == 1 || !CanNumberLexem(It_Count)){
		throw BadChar(std::string("Unknown Symbol in Line %d, Column %d", Line + 1, Column + 1).c_str());
	}
	SetToken(Line, Column, String.substr(It - String.cbegin() + 1, It_Count - It - 1), It_Count - It, TK_HEX_VALUE);
}

void Lexer::SetComment(const int Line, const int Column){
	std::string::iterator It_Count = It + 1;
	int Open_Brace_Count = 1;
	while (Open_Brace_Count > 0){
		if (*It_Count == EOF){
			throw BadEOF("Unexpected end of file"); 
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
	SetToken(Line, Column, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, NOT_TOKEN);
}

void Lexer::SetString(const int Line, const int Column){
	std::string::iterator It_Count = It + 1;
	while (*It_Count != '\'' && *It_Count != '\n' && *It_Count != EOF){
		++It_Count;
	}
	switch (*It_Count){
	case '\'':
		SetToken(Line, Column, String.substr(It - String.cbegin() + 1, It_Count - It - 1), It_Count - It + 1, TK_STRING_VALUE);
		break;
	case '\n': throw BadNL(std::string("Incorrect Symbol in Line %d Column %d", Line + 1, Column + 1).c_str());
	case EOF: throw BadEOF("Unexpected end of file"); 
	}
}

void Lexer::SetToken(const int Line, const int Column, const std::string Source, const int Length, const TokenType Type){
	TK.Set(Line, Column, Source, Type);
	It += Length;
}

void Lexer::Next(){
	try {
		if (!IsTokens)
			return;
		int Column = It - String.cbegin() + Column_Offset; 
		if ((*It < 0 || *It > 127) && *It != EOF){
			throw BadCC(std::string("Incorrect Symbol in Line %d, Column %d", Pos.Line + 1, Column + 1).c_str());
		}

		while (*It == ' ' || *It == '\t'){  /* Find Next Possible Token */
			if (*It == ' '){
				++Column;
			}
			else if (*It == '\t'){
				Column += 4;
				Column_Offset += 3;
			}
			++It;
		}

		if (*It >= '0' && *It <= '9'){
			SetNumberNotHex(Pos.Line, Column); /* set float or integer value */
			return;
		}
		else if (*It == '$'){
			SetNumberHex(Pos.Line, Column); /* set hex value */
			return;
		}
		else if (*It == '{'){
			SetComment(Pos.Line, Column); /* cut comment */
			return;
		}
		else if (String.cend() - It > 2 && *It == '/' && *(It + 1) == '/'){
			SetToken(Pos.Line, Column, "", 0, NOT_TOKEN); /* cut comment */
			this->NewString();
			return;
		}
		else if (*It == '\''){
			SetString(Pos.Line, Column); /* set string value */
			return;
		}
		else if (*It == '\n'){
			SetToken(Pos.Line, Column, "", 0, NOT_TOKEN); /* end string */
			this->NewString();
			return;
		}
		else if (*It == EOF){
			SetToken(Pos.Line, Column, String.substr(It - String.cbegin(), 1), 1, NOT_TOKEN); /* end file */
			IsTokens = false;
			return;
		}
		for (int i = 0; i < sizeof(Reserved_Words) / sizeof(TokenType); ++i){ /* Other Tokens from array 'Special_Symbols' */
			int len = strlen(Tokens_str[Reserved_Words[i]]);
			if (_strnicmp(String.substr(It - String.cbegin(), len).c_str(), Tokens_str[Reserved_Words[i]], len) == 0){
				SetToken(Pos.Line, Column, String.substr(It - String.cbegin(), len), len, Reserved_Words[i]);
				return;
			}
		}

		for (int i = 0; i < sizeof(Operators) / sizeof(TokenType); ++i){ /* Other Tokens from array 'Operators' */
			int len = strlen(Tokens_str[Operators[i]]);
			if (String.compare(It - String.cbegin(), len, Tokens_str[Operators[i]]) == 0){
				SetToken(Pos.Line, Column, String.substr(It - String.cbegin(), len), len, Operators[i]);
				return;
			}
		}

		std::string::iterator It_Count = It + 1; /* set identifier or error */
		if (!(*It >= 'a' && *It <= 'z' || *It >= 'A' && *It <= 'Z' || *It >= '0' && *It <= '9' || *It == '_')){
			throw BadNL(std::string("Incorrect Symbol in Line %d Column %d", Pos.Line + 1).c_str());
		}
		while (*It_Count >= 'a' && *It_Count <= 'z' || *It_Count >= 'A' && *It_Count <= 'Z' || *It_Count >= '0' && *It_Count <= '9' || *It_Count == '_'){
			++It_Count;
		}
		SetToken(Pos.Line, Column, String.substr(It - String.cbegin(), It_Count - It), It_Count - It, TK_IDENTIFIER);
	}
	catch (BadNL){ IsTokens = false; }
	catch (BadCC){ IsTokens = false; }
	catch (BadEOF){ IsTokens = false; }
	catch (BadChar){ IsTokens = false; }
	catch (BadExp){ IsTokens = false; }
	catch (NoFract){ IsTokens = false; }

}

void Lexer::Print(){
	if (TK.Type != NOT_TOKEN)
		std::cout << TK.Pos.Line << "\t" << TK.Pos.Column << "\t" << Tokens_str[TK.Type] << "\t" << TK.Source.c_str() << std::endl;
}

void Lexer::NewString(){
	if (String.length() > 0 && *String.crbegin() == EOF){
		IsTokens = false;
		return;
	}

	std::string NewStr;
	do 	NewStr += getc(f_in); while (*NewStr.crbegin() != EOF && *NewStr.crbegin() != '\n');
	String.assign(NewStr);
	this->It = String.begin();
	++Pos.Line;
	Pos.Column = 0;
	Column_Offset = 0;
}

Lexer::Lexer(const char* filename) : TK(), f_in(fopen(filename, "r")), IsTokens(true), Pos(-1, 0){
	NewString();
}

Lexer::Lexer(FILE *f) : TK(), f_in(f), IsTokens(true), Pos(-1, 0){
	NewString();
}