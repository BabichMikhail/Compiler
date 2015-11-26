#ifndef SYNTAXNODES_H
#define SYNTAXNODES_H

#include "lexer.h"
#include "errors.h"
#include <vector>
#include <set>
#include "utils.h"
#include <string>
#include "asmgenerator.h"

using namespace std;

class Symbol;
class Asm_Code;

enum ArgState { Null, Var, Const, Out };

#define indent "   "
#define print_indent(spaces) for (int i = 0; i < spaces; ++i) cout << indent

enum TypeExpr { BinExp, UnarExp, ConstIntExp, ConstDoubleExp, ConstBoolExp, ConstStringExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp, InitExp, 
	PointerExp, DereferenceExp };

class Expr{
public:
	TypeExpr TypeExp;
	Expr(TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces){};
	virtual void Generate(Asm_Code* Code, ArgState State = Null) {}
	virtual string GenerateInitList() { return ""; }
};

class ExprBinOp : public Expr{
public:
	Expr* Left;
	Token Op;
	Expr* Right;
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprUnarOp : public Expr{
public:
	Token Op;
	Expr* Exp;
	ExprUnarOp(Token Op, Expr* Exp);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprConst : public Expr{
public:
	Token Value;
	ExprConst(Token Value, TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces);
	virtual string GenerateInitList();
};

class ExprBoolConst : public ExprConst{
public:
	ExprBoolConst(Token Value);
	string GenerateInitList();
};

class ExprIntConst : public ExprConst{
public:
	ExprIntConst(Token Value);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprDoubleConst : public ExprConst{
public:
	ExprDoubleConst(Token Value);
};

class ExprStringConst : public ExprConst{
public:
	ExprStringConst(Token Value);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprIdent : public Expr{
public:
	Position Pos;
	Symbol* Sym;
	ExprIdent(Symbol* Sym, Position Pos);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprAssign : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprAssign(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprArrayIndex : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprArrayIndex(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprFunction : public Expr{
public:
	Expr* Left;
	vector<Expr*> Rights;
	ExprFunction(Expr* Left, vector<Expr*> Rights);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprRecord : public Expr{
public:
	Expr* Left;
	Symbol* Right;
	ExprRecord(Expr* Left, Symbol* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprInitList : public Expr {
public:
	vector<Expr*> List;
	ExprInitList(vector<Expr*> List = vector<Expr*>());
	void Print(const int Spaces);
	string GenerateInitList();
};

class ExprPointer : public Expr {
public:
	Expr* Exp;
	ExprPointer(Expr* Exp);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

class ExprDereference : public Expr {
public:
	Expr* Exp;
	ExprDereference(Expr* Exp);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, ArgState State = Null);
};

#endif