#ifndef STATEMENT_H
#define STATEMENT_H

using namespace std;

#include <vector>
#include "symbol.h"

class Statement{
public:
	virtual void Print(int Spaces);
};

class Stmt_GOTO : public Statement{
public:
	Symbol* LabelSym;
	Stmt_GOTO(Symbol* Sym);
	void Print(int Spaces);
};

class Stmt_Compound : public Statement{
public:
	vector<Statement*> StmtList;
	void Add(Statement* Stmt);
	void Print(int Spaces);
};

class Stmt_Conditional : public Statement{
public:
	Expr* Exp;
	Stmt_Conditional(Expr* Exp);
};

class Stmt_IF : public Stmt_Conditional{
public:
	Statement* Stmt;
	Statement* Stmt_Else;
	Stmt_IF(Expr* Exp, Statement* Stmt, Statement* Stmt_Else);
	void Print(int Spaces);
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
	Statement* Stmt_Else;
	vector<Case_Selector> SelectorList;
	void Add(Case_Selector Selector);
	Stmt_Case(Expr* Exp);
	void Print(int Spaces);
};

class Stmt_FOR : public Statement{
public:
	Expr* Exp_1;
	Expr* Exp_2;
	bool isTO;
	Statement* Stmt;
	Stmt_FOR(Expr* Exp_1, Expr* Exp_2, bool isTO, Statement* Stmt);
	void Print(int Spaces);
};

class Stmt_WHILE : public Statement {
public:
	Expr* Cond;
	Statement* Stmt;
	Stmt_WHILE(Expr* Cond, Statement* Stmt);
	void Print(int Spaces);
};

class Stmt_REPEAT : public Statement {
public:
	Expr* Cond;
	Statement* Stmt;
	Stmt_REPEAT(Expr* Cond, Statement* Stmt); 
	void Print(int Spaces);
};

class Stmt_BREAK : public Statement {
public:
	void Print(int Spaces);
};

class Stmt_Try_Except : public Statement{
public:
	Statement* Stmt_1;
	Statement* Stmt_2; // exp_bl
	Stmt_Try_Except(Statement* Stmt_1, Statement* Stmt_2);
	void Print(int Spaces);
};

class Stmt_Try_Finally : public Statement{
public:
	Statement* Stmt_1;
	Statement* Stmt_2;
	Stmt_Try_Finally(Statement* Stmt_1, Statement* Stmt_2);
	void Print(int Spaces);
};

#endif