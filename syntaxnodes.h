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

class OffsetNode {
public:
	OffsetNode(OffsetNode* Pref, int depth, int size);
	OffsetNode* Pref;
	int depth;
	int size;
};

enum ArgState { RValue, Var, Const, Out };

#define indent "   "
#define print_indent(spaces) for (int i = 0; i < spaces; ++i) cout << indent

enum TypeExpr { BinExp, UnarExp, ConstIntExp, ConstDoubleExp, ConstBoolExp, ConstStringExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp, InitExp, 
	PointerExp, DereferenceExp };

enum MyTypeID;

class Expr{
public:
	TypeExpr TypeExp;
	MyTypeID TypeID;
	Expr(TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces){};
	virtual void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue) {}
	virtual string GenerateInitList() { return ""; }
	virtual int GetSize() { return 0; };
	virtual pair<int, int> GetBound(int depth) { return make_pair(0, 0); };
};

class ExprBinOp : public Expr{
public:
	Expr* Left;
	Token Op;
	Expr* Right;
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
};

class ExprUnarOp : public Expr{
public:
	Token Op;
	Expr* Exp;
	ExprUnarOp(Token Op, Expr* Exp);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
};

class ExprConst : public Expr{
public:
	Token Value;
	ExprConst(Token Value, TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces);
	virtual string GenerateInitList();
	int GetSize();
};

class ExprBoolConst : public ExprConst{
public:
	ExprBoolConst(Token Value);
	string GenerateInitList();
};

class ExprIntConst : public ExprConst{
public:
	ExprIntConst(Token Value);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
};

class ExprDoubleConst : public ExprConst{
public:
	ExprDoubleConst(Token Value);
};

class ExprStringConst : public ExprConst{
public:
	ExprStringConst(Token Value);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
};

class ExprIdent : public Expr{
public:
	Position Pos;
	Symbol* Sym;
	ExprIdent(Symbol* Sym, Position Pos);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
	pair<int, int> GetBound(int depth);
};

class ExprAssign : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprAssign(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
};

class ExprArrayIndex : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprArrayIndex(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
	pair<int, int> GetBound(int depth);
};

class ExprFunction : public Expr{
public:
	Expr* Left;
	vector<Expr*> Rights;
	ExprFunction(Expr* Left, vector<Expr*> Rights);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
	pair<int, int> GetBound(int depth);
};

class ExprRecord : public Expr{
public:
	Expr* Left;
	Symbol* Right;
	ExprRecord(Expr* Left, Symbol* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
	pair<int, int> GetBound(int depth);
};

class ExprInitList : public Expr {
public:
	vector<Expr*> List;
	ExprInitList(vector<Expr*> List = vector<Expr*>());
	void Print(const int Spaces);
	string GenerateInitList();
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
};

class ExprPointer : public Expr {
public:
	Expr* Exp;
	ExprPointer(Expr* Exp);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
};

class ExprDereference : public Expr {
public:
	Expr* Exp;
	ExprDereference(Expr* Exp);
	void Print(const int Spaces);
	void Generate(Asm_Code* Code, OffsetNode* Offsets, ArgState State = RValue);
	int GetSize();
	pair<int, int> GetBound(int depth);
};

#endif