#ifndef SYNTAXNODES_H
#define SYNTAXNODES_H

#include "lexer.h"
#include "errors.h"
#include <vector>
#include <set>
#include "utils.h"
#include <string>

using namespace std;

class Symbol;
class Asm_Code;

#define indent "   "
#define print_indent(spaces) for (int i = 0; i < spaces; ++i) cout << indent

enum AsmOp { AsmPush = 0, AsmPop, AsmIMul, AsmDiv, AsmAdd, AsmSub, AsmNeg, AsmNot, AsmOr, AsmAnd, AsmXor, AsmShl, AsmShr, AsmCall, AsmMov };
enum AsmRegistr { AsmEAX = 0, AsmEBX, AsmECX, AsmEDX, AsmEBP, AsmESP };
enum TypeExpr { BinExp, UnarExp, ConstIntExp, ConstDoubleExp, ConstBoolExp, ConstStringExp, VarExp, ArrayExp, AssignExp, FunctionExp, RecordExp, InitExp, 
	PointerExp, DereferenceExp };

class Expr{
public:
	TypeExpr TypeExp;
	Expr(TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces){};
	virtual vector<Asm_Code*> GetAsmCode() { return vector<Asm_Code*>(); }
};

class ExprBinOp : public Expr{
public:
	Expr* Left;
	Token Op;
	Expr* Right;
	ExprBinOp(Expr* Left, Token Op, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	vector<Asm_Code*> GetAsmCode();
};

class ExprUnarOp : public Expr{
public:
	Token Op;
	Expr* Exp;
	ExprUnarOp(Token Op, Expr* Exp);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	vector<Asm_Code*> GetAsmCode();
};

class ExprConst : public Expr{
public:
	Token Value;
	ExprConst(Token Value, TypeExpr TypeExp);
	virtual void GetIdentStr(ExpArgList* List);
	virtual void Print(const int Spaces);
};

class ExprBoolConst : public ExprConst{
public:
	ExprBoolConst(Token Value);
};

class ExprIntConst : public ExprConst{
public:
	ExprIntConst(Token Value);
	vector<Asm_Code*> GetAsmCode();
};

class ExprDoubleConst : public ExprConst{
public:
	ExprDoubleConst(Token Value);
};

class ExprStringConst : public ExprConst{
public:
	ExprStringConst(Token Value);
	vector<Asm_Code*> GetAsmCode();
};

class ExprVar : public Expr{
public:
	Symbol* Sym;
	ExprVar(Symbol* Sym);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprAssign : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprAssign(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprArrayIndex : public Expr{
public:
	Expr* Left;
	Expr* Right;
	ExprArrayIndex(Expr* Left, Expr* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprFunction : public Expr{
public:
	Expr* Left;
	vector<Expr*> Rights;
	ExprFunction(Expr* Left, vector<Expr*> Rights);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
	vector<Asm_Code*> GetAsmCode();
};

class ExprRecord : public Expr{
public:
	Expr* Left;
	Symbol* Right;
	ExprRecord(Expr* Left, Symbol* Right);
	void GetIdentStr(ExpArgList* List);
	void Print(const int Spaces);
};

class ExprInitList : public Expr {
public:
	vector<Expr*> List;
	ExprInitList(vector<Expr*> List = vector<Expr*>());
	void Print(const int Spaces);
};

class ExprPointer : public Expr {
public:
	Expr* Exp;
	ExprPointer(Expr* Exp);
	void Print(const int Spaces);
};

class ExprDereference : public Expr {
public:
	Expr* Exp;
	ExprDereference(Expr* Exp);
	void Print(const int Spaces);
};

#endif