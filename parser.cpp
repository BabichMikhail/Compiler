#include "parser.h"
#include <iostream>
#include "errors.h"
#include <set>

using namespace std;

#define indent "   "
#define CheckAssign(Right, c) if (Right->IndType == AssignExp) throw UnexpectedSymbol(c, ":=")

enum TypeExpr { BinExp, UnarExp, ConstExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp };

auto start_expr_tk = { TK_INTEGER_VALUE, TK_REAL_VALUE, TK_OPEN_BRACKET, TK_IDENTIFIER, TK_PLUS, TK_MINUS, TK_TRUE, TK_FALSE, TK_NOT };
auto rel_op = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };
auto add_op = { TK_PLUS, TK_MINUS, TK_OR, TK_XOR };
auto mul_op = { TK_MUL, TK_DIV, TK_DIV_INT, TK_MOD, TK_AND, TK_SHL, TK_SHR };
auto simp_stat = { TK_ASSIGNED, TK_OPEN_SQUARE_BRACKET, TK_OPEN_BRACKET, TK_POINT };
set<TokenType> vec_start_expr_tk(start_expr_tk), vec_rel_op(rel_op), vec_add_op(add_op), vec_mul_op(mul_op);

Expr::Expr(int IT): IndType(IT){}

ExprBinOp::ExprBinOp(Expr* Left, Token Op, Expr* Right) : Left(Left), Op(Op), Right(Right), Expr(BinExp){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp), Expr(UnarExp){}

ExprConst::ExprConst(Token Value) : Value(Value), Expr(ConstExp){}
ExprBoolConst::ExprBoolConst(Token Value) : ExprConst(Value){}
ExprIntConst::ExprIntConst(Token Value) : ExprConst(Value){}
ExprRealConst::ExprRealConst(Token Value) : ExprConst(Value){}

ExprVar::ExprVar(Token Var) : Var(Var), Expr(VarExp){}

ArrayIndex::ArrayIndex(ExprVar* Left, Expr* Right) : Left(Left), Right(Right), Expr(ArrayExp){};
Assign::Assign(ExprVar* Left, Expr* Right) : Left(Left), Right(Right), Expr(AssignExp){}
Function::Function(ExprVar* Left, vector<Expr*> Rights) : Left(Left), Rights(Rights), Expr(FunctionExp){}
Record::Record(Expr* Left, ExprVar* Right) : Left(Left), Right(Right), Expr(RecordExp){}

void ExprUnarOp::Print(int Spaces){
	this->Exp->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << this->Op.Source.c_str() << endl;
}

void ExprBinOp::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << this->Op.Source.c_str() << endl;
	this->Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << this->Value.Source.c_str() << endl;
}

void ExprVar::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << this->Var.Source.c_str() << endl;
}

void Assign::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << ":="<< endl;
	this->Left->Print(Spaces + 1);
}

void Function::Print(int Spaces){
	for (int i = 0; i < Rights.size(); ++i){
		this->Rights[i]->Print(Spaces + 1);
	}
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << "()" << endl;
	this->Left->Print(Spaces + 1);
}

void Record::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << "." << endl;
	this->Left->Print(Spaces + 1);
}

void ArrayIndex::Print(int Spaces){
	this->Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		std::cout << indent;
	}
	cout << "[]" << endl;
	this->Left->Print(Spaces + 1);
}

void Parser::Print(){
	this->Exp->Print(0);
}

Parser::Parser(const char* filename) : Lex(filename){
	while (Lex.isToken()){
		Lex.Next();
		if (vec_start_expr_tk.find(Lex.Get().Type) != vec_start_expr_tk.end()){
			Exp = ParseExprByParam(St_Parse_Expr);
			continue;
		}
	}
}

Expr* Parser::ParseExprByParam(PState State){
	if (State == St_Parse_Expr){
		auto Left = ParseExprByParam(St_Parse_Simple_Expr);
		while (vec_rel_op.find(Lex.Get().Type) != vec_rel_op.end()){
			Token Op = Lex.Get();
			Lex.Next();
			auto Right = ParseExprByParam(St_Parse_Simple_Expr);
			CheckAssign(Right, ";");
			Left = (Expr*)new ExprBinOp(Left, Op, Right);
		}
		return Left;
	}

	if (State == St_Parse_Simple_Expr){
		auto Left = ParseExprByParam(St_Parse_Term);
		while (vec_add_op.find(Lex.Get().Type) != vec_add_op.end()){
			Token Op = Lex.Get();
			Lex.Next();
			auto Right = ParseExprByParam(St_Parse_Term);
			CheckAssign(Right, ";");
			Left = (Expr*)new ExprBinOp(Left, Op, Right);
		}
		return Left;
	}

	if (State == St_Parse_Term){
		auto Left = ParseExprByParam(St_Parse_Factor);
		while (vec_mul_op.find(Lex.Get().Type) != vec_mul_op.end()){
			Token Op = Lex.Get();
			Lex.Next();
			auto Right = ParseExprByParam(St_Parse_Factor);
			CheckAssign(Right, ";");
			Left = (Expr*)new ExprBinOp(Left, Op, Right);
		}
		return Left;
	}

	if (State == St_Parse_Factor){
		auto TK = Lex.Get();
		if (TK.Type == TK_IDENTIFIER){
			auto ExpNow = ParseIdentifier();
			return ExpNow;
		}
		Lex.Next();
		if (TK.Type == TK_MINUS || TK.Type == TK_PLUS){
			auto ExpNow = ParseExprByParam(St_Parse_Expr);
			return (Expr*)new ExprUnarOp(TK, ExpNow);
		}
		if (TK.Type == TK_OPEN_BRACKET){
			auto ExpNow = ParseExprByParam(St_Parse_Expr);
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw UnexpectedSymbol(")", Lex.Get().Source);
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
		if (TK.Type == TK_NOT){
			auto ExpNow = ParseExprByParam(St_Parse_Factor);
			return (Expr*)new ExprUnarOp(TK, ExpNow);
		}
		throw IllegalExpr();
	}
}

Expr* Parser::ParseIdentifier(){
	auto TK = Lex.Get();
	Lex.Next();
	auto ExpNow = (Expr*)new ExprVar(TK);
	while (Lex.Get().Type == TK_ASSIGNED || Lex.Get().Type == TK_OPEN_SQUARE_BRACKET || Lex.Get().Type == TK_OPEN_BRACKET || Lex.Get().Type == TK_POINT){
		if (Lex.Get().Type == TK_ASSIGNED){
			Lex.Next();
			auto Right = ParseExprByParam(St_Parse_Expr);
			CheckAssign(Right, ";");
			ExpNow = (Expr*)new Assign((ExprVar*)ExpNow, Right);
		}
		else if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
			Lex.Next();
			auto Right = ParseExprByParam(St_Parse_Expr);
			CheckAssign(Right, "]");
			ExpNow = (Expr*)new ArrayIndex((ExprVar*)ExpNow, Right);
			while (Lex.Get().Type == TK_COMMA){
				Lex.Next();
				Right = ParseExprByParam(St_Parse_Expr);
				CheckAssign(Right, "]");
				ExpNow = (Expr*)new ArrayIndex((ExprVar*)ExpNow, Right);
			}
			if (Lex.Get().Type != TK_CLOSE_SQUARE_BRACKET){
				throw UnexpectedSymbol("]", Lex.Get().Source);
			}
			Lex.Next();
		}
		else if (Lex.Get().Type == TK_OPEN_BRACKET){
			vector<Expr*> Rights;
			Lex.Next();
			Rights.push_back(ParseExprByParam(St_Parse_Expr));
			CheckAssign(Rights[0], ")");
			while (Lex.Get().Type == TK_COMMA){
				Lex.Next();
				Rights.push_back(ParseExprByParam(St_Parse_Expr));
				CheckAssign(Rights[Rights.size() - 1], ")");
			}
			ExpNow = (Expr*)new Function((ExprVar*)ExpNow, Rights);
		}
		else if (Lex.Get().Type == TK_POINT){
			Lex.Next();
			if (Lex.Get().Type != TK_IDENTIFIER){
				throw UnexpectedSymbol("identifier", Lex.Get().Source);
			}
			auto Right = (Expr*)new ExprVar(Lex.Get());
			Lex.Next();
			ExpNow = (Expr*)new Record(ExpNow, (ExprVar*)Right);
		}
	}
	return ExpNow;
}
