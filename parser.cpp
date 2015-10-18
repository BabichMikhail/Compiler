#include "parser.h"
#include "errors.h"

using namespace std;

#define indent "   "
#define parse_factor_level 3

set<TokenType> start_expr_tk = { TK_INTEGER_VALUE, TK_REAL_VALUE, TK_OPEN_BRACKET, TK_IDENTIFIER, TK_PLUS, TK_MINUS, TK_TRUE, TK_FALSE, TK_NOT };

set<TokenType> rel_op = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };
set<TokenType> add_op = { TK_PLUS, TK_MINUS, TK_OR, TK_XOR };
set<TokenType> mul_op = { TK_MUL, TK_DIV, TK_DIV_INT, TK_MOD, TK_AND, TK_SHL, TK_SHR };
set<TokenType> simp_stat = { TK_OPEN_SQUARE_BRACKET, TK_OPEN_BRACKET, TK_POINT };
vector<set<TokenType>> level_list = { rel_op, add_op, mul_op };

set<TypeExpr> Left_Op_Assign = { VarExp, RecordExp, ArrayExp };

Expr::Expr(TypeExpr TypeExp) : TypeExp(TypeExp){}

ExprBinOp::ExprBinOp(Expr* Left, Token Op, Expr* Right) : Left(Left), Op(Op), Right(Right), Expr(BinExp){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp), Expr(UnarExp){}

ExprConst::ExprConst(Token Value) : Value(Value), Expr(ConstExp){}
ExprBoolConst::ExprBoolConst(Token Value) : ExprConst(Value){}
ExprIntConst::ExprIntConst(Token Value) : ExprConst(Value){}
ExprRealConst::ExprRealConst(Token Value) : ExprConst(Value){}

ExprVar::ExprVar(Token Var) : Var(Var), Expr(VarExp){}

ArrayIndex::ArrayIndex(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(ArrayExp){}
Assign::Assign(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(AssignExp){}
Function::Function(Expr* Left, vector<Expr*> Rights) : Left(Left), Rights(Rights), Expr(FunctionExp){}
Record::Record(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(RecordExp){}

void ExprUnarOp::Print(int Spaces){
	Exp->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Op.Source.c_str() << endl;
}

void ExprBinOp::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Op.Source.c_str() << endl;
	Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Value.Source.c_str() << endl;
}

void ExprVar::Print(int Spaces){
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << Var.Source.c_str() << endl;
}

void Assign::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << ":="<< endl;
	Left->Print(Spaces + 1);
}

void Function::Print(int Spaces){
	for (int i = 0; i < Rights.size(); ++i){
		Rights[i]->Print(Spaces + 1);
	}
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "()" << endl;
	Left->Print(Spaces + 1);
}

void Record::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "." << endl;
	Left->Print(Spaces + 1);
}

void ArrayIndex::Print(int Spaces){
	Right->Print(Spaces + 1);
	for (int i = 0; i < Spaces; ++i){
		cout << indent;
	}
	cout << "[]" << endl;
	Left->Print(Spaces + 1);
}

void Parser::Print(){
	Exp->Print(0);
}

Parser::Parser(const char* filename) : Lex(filename){
	while (Lex.isToken()){
		Lex.Next(); 
		if (start_expr_tk.find(Lex.Get().Type) != start_expr_tk.end()){
			Exp = ParseExpr();
			//continue;
		}
	}
}

Expr* Parser::ParseExpr(){
	auto Left = ParseLevel(0);
	auto TK = Lex.Get();
	if (TK.Type == TK_ASSIGNED){
		Lex.Next();
		auto Right = ParseLevel(0);
		if (Left_Op_Assign.find(Left->TypeExp) == Left_Op_Assign.cend()){
			throw ExpectedVariable();
		}
		return new Assign(Left, Right);
	}
	return Left;
}

Expr* Parser::ParseLevel(const int level){
	if (level == parse_factor_level){
		return ParseFactor();
	}
	auto Left = ParseLevel(level + 1);
	while (level_list[level].find(Lex.Get().Type) != level_list[level].cend()){
		Token Op = Lex.Get();
		Lex.Next();
		Left = new ExprBinOp(Left, Op, ParseLevel(level + 1));
	}
	return Left;
}

Expr* Parser::ParseFactor(){
	auto TK = Lex.Get();
	if (TK.Type == NOT_TOKEN){
		throw IllegalExpr();
	}
	if (TK.Type == TK_IDENTIFIER){
		return ParseDesignator();
	}
	Lex.Next();
	if (TK.Type == TK_MINUS || TK.Type == TK_PLUS){
		return new ExprUnarOp(TK, ParseLevel(0));
	}
	if (TK.Type == TK_OPEN_BRACKET){
		auto ExpNow = ParseLevel(0);
		if (Lex.Get().Type != TK_CLOSE_BRACKET){
			throw UnexpectedSymbol(")", Lex.Get().Source);
		}
		Lex.Next();
		return ExpNow;
	}
	if (TK.Type == TK_INTEGER_VALUE){
		return new ExprIntConst(TK);
	}
	if (TK.Type == TK_REAL_VALUE){
		return new ExprRealConst(TK);
	}
	if (TK.Type == TK_TRUE || TK.Type == TK_FALSE){
		return new ExprBoolConst(TK);
	}
	if (TK.Type == TK_NOT){
		return new ExprUnarOp(TK, ParseLevel(0));
	}
	throw IllegalExpr();
}

Expr* Parser::ParseDesignator(){
	auto TK = Lex.Get();
	Lex.Next();
	Expr* ExpNow = new ExprVar(TK);
	while (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET || Lex.Get().Type == TK_OPEN_BRACKET || Lex.Get().Type == TK_POINT){
		if (Lex.Get().Type == TK_OPEN_SQUARE_BRACKET){
			do {
				Lex.Next();
				ExpNow = new ArrayIndex(ExpNow, ParseLevel(0));
			} while (Lex.Get().Type == TK_COMMA);
			if (Lex.Get().Type != TK_CLOSE_SQUARE_BRACKET){
				throw UnexpectedSymbol("]", Lex.Get().Source);
			}
			Lex.Next();
		}
		if (Lex.Get().Type == TK_OPEN_BRACKET){
			vector<Expr*> Arguments;
			Lex.Next();
			if (Lex.Get().Type != TK_CLOSE_BRACKET){ /* ")" == no argument */
				Arguments.push_back(ParseLevel(0));
				while (Lex.Get().Type == TK_COMMA){
					Lex.Next();
					Arguments.push_back(ParseLevel(0));
				} 
			}
			if (Lex.Get().Type != TK_CLOSE_BRACKET){
				throw UnexpectedSymbol(")", Lex.Get().Source);
			}
			Lex.Next();
			ExpNow = new Function(ExpNow, Arguments);
		}
		if (Lex.Get().Type == TK_POINT){
			Lex.Next();
			if (Lex.Get().Type != TK_IDENTIFIER){
				throw UnexpectedSymbol("identifier", Lex.Get().Source);
			}
			auto Right = new ExprVar(Lex.Get());
			Lex.Next();
			ExpNow = new Record(ExpNow, Right);
		}
	}
	return ExpNow;
}
