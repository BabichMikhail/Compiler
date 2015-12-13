#ifndef STATEMENT_H
#define STATEMENT_H

using namespace std;

#include <vector>
#include "symbol.h"

class Statement{
public:
	virtual void Print(int Spaces) {};
	virtual void Generate(Asm_Code* Code) {};
};

class Stmt_Goto : public Statement{
public:
	Symbol* LabelSym;
	Stmt_Goto(Symbol* Sym);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Goto_Label : public Statement {
public:
	Symbol* LabelSym;
	Stmt_Goto_Label(Symbol* Sym);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Compound : public Statement{
public:
	vector<Statement*> StmtList;
	void Add(Statement* Stmt);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Conditional : public Statement{
public:
	Expr* Exp;
	Stmt_Conditional(Expr* Exp);
	void Generate_Condition(Asm_Code* Code);
};

class Stmt_If : public Stmt_Conditional{
public:
	Statement* Stmt;
	Statement* StmtElse;
	Stmt_If(Expr* Exp, Statement* Stmt, Statement* Stmt_Else);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Case_Selector {
public:
	Expr* Exp_1;
	Expr* Exp_2;
	Statement* Stmt;
	Case_Selector(Expr* Exp_1, Expr* Exp_2, Statement* Stmt);
};

class Stmt_Case : public Stmt_Conditional{
public:
	Statement* StmtElse;
	vector<Case_Selector> SelectorList;
	void Add(Case_Selector Selector);
	Stmt_Case(Expr* Exp);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_For : public Stmt_Conditional{
public:
	Expr* Exp_1;
	Expr* Exp_2;
	bool isTO;
	Statement* Stmt;
	Stmt_For(Expr* Exp_1, Expr* Exp_2, bool isTO, Statement* Stmt);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_While : public Stmt_Conditional {
public:
	Statement* Stmt;
	Stmt_While(Expr* Cond, Statement* Stmt);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Repeat : public Stmt_Conditional {
public:
	vector<Statement*> StmtList;
	Stmt_Repeat(Expr* Cond, vector<Statement*> Stmt_List); 
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Break : public Statement {
public:
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Continue : public Statement {
public:
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Try_Except : public Statement{
public:
	vector<Statement*> Stmt_List_Try;
	vector<Statement*> Stmt_List_Except;
	Stmt_Try_Except(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Except);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Try_Finally : public Statement{
public:
	vector<Statement*> Stmt_List_Try;
	vector<Statement*> Stmt_List_Finally;
	Stmt_Try_Finally(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Finally);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Raise : public Statement {
public:
	Expr* Exp;
	Stmt_Raise(Expr* Exp);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Assign : public Statement {
public:
	Expr* Exp;
	Stmt_Assign(Expr* Exp);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

class Stmt_Call : public Statement {
public:
	Expr* Exp;
	Stmt_Call(Expr* Exp);
	void Print(int Spaces);
	void Generate(Asm_Code* Code);
};

#endif