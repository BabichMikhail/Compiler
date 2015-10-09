#include "parser.h"
#include <iostream>
#include "errors.h"
#include <vector>

#define indent "   "

auto start_expr_tk = { TK_INTEGER_VALUE, TK_REAL_VALUE, TK_OPEN_BRACKET, TK_IDENTIFIER, TK_PLUS, TK_MINUS, TK_TRUE, TK_FALSE, TK_NOT };
auto rel_op = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };
auto add_op = { TK_PLUS, TK_MINUS, TK_OR, TK_XOR };
auto mul_op = { TK_MUL, TK_DIV, TK_DIV_INT, TK_MOD, TK_AND, TK_SHL, TK_SHR };
std::vector<TokenType> vec_start_expr_tk(start_expr_tk), vec_rel_op(rel_op), vec_add_op(add_op), vec_mul_op(mul_op);

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
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	std::cout << this->Op.Source.c_str() << std::endl;
}

void ExprBinOp::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	std::cout << this->Op.Source.c_str() << std::endl;
	this->Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	std::cout << this->Val.Source.c_str() << std::endl;
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
			if (std::find(vec_start_expr_tk.begin(), vec_start_expr_tk.end(), Lex.Get().Type) != vec_start_expr_tk.end()){
				Exp = ParseExpr();
				continue;
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
	while (std::find(vec_rel_op.begin(), vec_rel_op.end(), Lex.Get().Type) != vec_rel_op.end()){
		Token Op = Lex.Get();
		Lex.Next();
		auto Right = ParseSimpleExpr();
		Left = (Expr*)new ExprBinOp(Op, Left, Right);
	}
	return Left;
}

Expr* Parser::ParseSimpleExpr(){
	auto Left = ParseTerm();
	while (std::find(vec_add_op.begin(), vec_add_op.end(), Lex.Get().Type) != vec_add_op.end()){
		Token Op = Lex.Get();
		Lex.Next();
		auto Right = ParseTerm();
		Left = (Expr*)new ExprBinOp(Op, Left, Right);
	}
	return Left;
}

Expr* Parser::ParseTerm(){
	auto Left = ParseFactor();
	while (std::find(vec_mul_op.begin(), vec_mul_op.end(), Lex.Get().Type) != vec_mul_op.end()){
		Token Op = Lex.Get();
		Lex.Next();
		auto Right = ParseFactor();
		Left = (Expr*)new ExprBinOp(Op, Left, Right);
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