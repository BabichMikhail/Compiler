#include "parser.h"
#include <iostream>
#include "errors.h"

#define indent "   "

Expr::Expr(){}

ExprBinOp::ExprBinOp(Token Op, Expr* L, Expr* R) : Op(Op), Left(L), Right(R){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp){}

ExprConst::ExprConst(Token Val) : Val(Val){}
ExprBoolConst::ExprBoolConst(Token Val) : ExprConst(Val){}
ExprIntConst::ExprIntConst(Token Val) : ExprConst(Val){}
ExprRealConst::ExprRealConst(Token Val) : ExprConst(Val){}
ExprVar::ExprVar(Token Var) : ExprConst(Var){}


void ExprUnarOp::Print(int Spaces){
	this->Exp->Print(Spaces + 1);
	std::string S;
	for (int i = 0; i < Spaces; ++i){
		S += indent;
	}
	std::cout << S.c_str() << this->Op.Source.c_str() << std::endl;
}

void ExprBinOp::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	std::string S;
	for (int i = 0; i < Spaces; ++i){
		S += indent;
	}
	std::cout << S.c_str() <<  this->Op.Source.c_str() << std::endl;
	this->Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	std::string S;
	for (int i = 0; i < Spaces; ++i){
		S += indent;
	}
	std::cout << S.c_str() << this->Val.Source.c_str() << std::endl;
}

void Parser::Print(){
	if (State == St_Bad){
		BadEOF("Syntax error: expected expression but \"end of file\" found");
		return;
	}
	Exp->Print(0);
}

Parser::Parser(const char* filename) : Lex(filename), State(St_Good){
	try {
		while (Lex.isToken()){
			Lex.Next();
			if (Lex.Get().Type == TK_INTEGER_VALUE || Lex.Get().Type == TK_REAL_VALUE || Lex.Get().Type == TK_OPEN_BRACKET || 
				Lex.Get().Type == TK_IDENTIFIER || Lex.Get().Type == TK_PLUS || Lex.Get().Type == TK_MINUS || Lex.Get().Type == TK_TRUE || 
				Lex.Get().Type == TK_FALSE || Lex.Get().Type == TK_NOT){
				Exp = ParseExpr();
				return;
			}
			/*if (Lex.Get().Type == TK_IF){}
			if (Lex.Get().Type == TK_BEGIN){}
			if (Lex.Get().Type == TK_PROGRAM){}
			if (Lex.Get().Type == TK_PROCEDURE){}
			if (Lex.Get().Type == TK_FUNCTION){}
			if (Lex.Get().Type == TK_VAR){}
			if (Lex.Get().Type == TK_CONST){}
			if (Lex.Get().Type == TK_FOR){}
			if (Lex.Get().Type == TK_WHILE){}
			if (Lex.Get().Type == TK_GOTO){}
			if (Lex.Get().Type == TK_RECORD){}
			if (Lex.Get().Type == TK_REPEAT){}*/
			//throw IllegalExpr("Error: Illegal Expression");
		}
	}
	catch (AbsentBrackect){ State = St_Bad; }
	catch (IllegalExpr){ State = St_Bad; }
}

Expr* Parser::ParseExpr(){
	auto Left = ParseSimpleExpr();
	while (Lex.Get().Type == TK_GREAT || Lex.Get().Type == TK_GREAT_EQUAL || Lex.Get().Type == TK_LESS || Lex.Get().Type == TK_LESS_EQUAL
		|| Lex.Get().Type == TK_EQUAL || Lex.Get().Type == TK_NOT_EQUAL){

		Token Op = Lex.Get();
		Lex.Next();
		if (Lex.Get().Type == TK_OPEN_BRACKET){
			Lex.Next();
			auto Right = ParseExpr();
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw AbsentBrackect("Fatal: Syntax error, ')' excepted but \"" + Lex.Get().Source + "\" found");
			}
			Lex.Next();
			if (Lex.Get().Type == TK_PLUS || Lex.Get().Type == TK_MINUS || Lex.Get().Type == TK_OR || Lex.Get().Type == TK_XOR){
				auto Op_2 = Lex.Get();
				Lex.Next();
				Left = (Expr*)new ExprBinOp(Op, Left, (Expr*)new ExprBinOp(Op_2, Right, ParseSimpleExpr()));
			}
			else 
				Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
		else {
			auto Right = ParseSimpleExpr();
			Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
	}
	return Left;
}

Expr* Parser::ParseSimpleExpr(){
	auto Left = ParseTerm();
	while (Lex.Get().Type == TK_PLUS || Lex.Get().Type == TK_MINUS || Lex.Get().Type == TK_OR || Lex.Get().Type == TK_XOR){
		Token Op = Lex.Get();
		Lex.Next();
		if (Lex.Get().Type == TK_OPEN_BRACKET){
			Lex.Next();
			auto Right = ParseExpr();
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw AbsentBrackect("Fatal: Syntax error, ')' excepted but \"" + Lex.Get().Source + "\" found");
			}
			Lex.Next();
			if (Lex.Get().Type == TK_MUL || Lex.Get().Type == TK_DIV || Lex.Get().Type == TK_DIV_INT || Lex.Get().Type == TK_MOD || Lex.Get().Type == TK_AND
				|| Lex.Get().Type == TK_SHL || Lex.Get().Type == TK_SHR){

				auto Op_2 = Lex.Get();
				Lex.Next();
				Left = (Expr*)new ExprBinOp(Op, Left, (Expr*)new ExprBinOp(Op_2, Right, ParseTerm()));
			}
			else
				Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
		else {
			auto Right = ParseTerm();
			Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
	}
	return Left;
}

Expr* Parser::ParseTerm(){
	auto Left = ParseFactor();
	while (Lex.Get().Type == TK_MUL || Lex.Get().Type == TK_DIV || Lex.Get().Type == TK_DIV_INT || Lex.Get().Type == TK_MOD || Lex.Get().Type == TK_AND 
		|| Lex.Get().Type == TK_SHL || Lex.Get().Type == TK_SHR){

		Token Op = Lex.Get();
		Lex.Next();
		if (Lex.Get().Type == TK_OPEN_BRACKET){
			Lex.Next();
			auto Right = ParseExpr();
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw AbsentBrackect("Fatal: Syntax error, ')' excepted but \"" + Lex.Get().Source + "\" found");
			}
			Lex.Next();
			Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
		else {
			auto Right = ParseFactor();
			Left = (Expr*)new ExprBinOp(Op, Left, Right);
		}
	}
	return Left;
}

Expr* Parser::ParseFactor(){
	auto TK = Lex.Get();
	Lex.Next();
	if (TK.Type == TK_MINUS || TK.Type == TK_PLUS){
		auto ExpNow = ParseExpr();
		return (Expr*)new ExprUnarOp(TK, ExpNow);
	}
	if (TK.Type == TK_OPEN_BRACKET){
		auto ExpNow = ParseExpr();
		if (Lex.Get().Type != TK_CLOSE_BRACKET){
			throw AbsentBrackect("Fatal: Syntax error, ')' excepted but \"" + Lex.Get().Source + "\" found");
		}
		Lex.Next();
		return ExpNow;
	}
	if (TK.Type == TK_INTEGER_VALUE){
		return (Expr*)new ExprIntConst(TK);
	}
	if (TK.Type == TK_REAL_VALUE){
		return (Expr*)new ExprRealConst(TK);
	}
	if (TK.Type == TK_TRUE || TK.Type == TK_FALSE){
		return (Expr*)new ExprBoolConst(TK);
	}
	if (TK.Type == TK_IDENTIFIER){
		return (Expr*)new ExprVar(TK);
	}
	if (TK.Type == TK_NOT){
		auto ExpNow = ParseFactor();
		return (Expr*)new ExprUnarOp(TK, ExpNow);
	}
	throw IllegalExpr("Error: Illegal Expression");
}