#include "statement.h"

Stmt_GOTO::Stmt_GOTO(Symbol* Sym) : LabelSym(Sym){};
Stmt_Conditional::Stmt_Conditional(Expr* Exp) : Exp(Exp){};
Stmt_IF::Stmt_IF(Expr* Exp, Statement* Stmt, Statement* Stmt_Else) : Stmt_Conditional(Exp), Stmt(Stmt), Stmt_Else(Stmt_Else){}

Stmt_FOR::Stmt_FOR(Expr* Exp_1, Expr* Exp_2, bool isTO, Statement* Stmt) : Exp_1(Exp_1), Exp_2(Exp_2), isTO(isTO), Stmt(Stmt){};
Stmt_WHILE::Stmt_WHILE(Expr* Cond, Statement* Stmt) : Cond(Cond), Stmt(Stmt) {};
Stmt_REPEAT::Stmt_REPEAT(Expr* Cond, vector<Statement*> Stmt_List) : Cond(Cond), Stmt_List(Stmt_List) {};

Case_Selector::Case_Selector(Expr* Exp_1, Expr* Exp_2, Statement* Stmt) : Exp_1(Exp_1), Exp_2(Exp_2), Stmt(Stmt) {};
Stmt_Case::Stmt_Case(Expr* Exp) : Stmt_Conditional(Exp), Stmt_Else(nullptr) {};

Stmt_Try_Except::Stmt_Try_Except(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Except) : Stmt_List_Try(Stmt_Try), Stmt_List_Except(Stmt_Except){}
Stmt_Try_Finally::Stmt_Try_Finally(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Finally) : Stmt_List_Try(Stmt_Try), Stmt_List_Finally(Stmt_Finally){}

Stmt_Raise::Stmt_Raise(Expr* Exp) : Exp(Exp) {};

Stmt_Assign::Stmt_Assign(Expr* Exp) : Exp(Exp) {};

void Stmt_Compound::Add(Statement* Stmt){
	StmtList.push_back(Stmt);
}

void Stmt_Case::Add(Case_Selector Selector) {
	SelectorList.push_back(Selector);
}

void Statement::Print(int Spaces) {
	
}

void Stmt_Compound::Print(int Spaces) {
	print_indent(Spaces);
	cout << "begin" << endl;
	for (int i = 0; i < StmtList.size(); ++i) {
		StmtList[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "end" << endl;
}

void Stmt_GOTO::Print(int Spaces) {
	print_indent(Spaces);
	cout << "goto" << indent << LabelSym->Name << endl;
}

void Stmt_IF::Print(int Spaces){
	print_indent(Spaces);
	cout << "if" << endl;
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "then" << endl;
	Stmt->Print(Spaces + 1);
	if (Stmt_Else != nullptr) {
		print_indent(Spaces);
		cout << "else" << endl;
		Stmt_Else->Print(Spaces + 1);
	}
}

void Stmt_FOR::Print(int Spaces){
	print_indent(Spaces);
	cout << "for" << endl;
	Exp_1->Print(Spaces + 1);
	print_indent(Spaces + 1);
	isTO ? cout << "true" << endl : cout << "false" << endl;
	Exp_2->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "do" << endl;
	if (Stmt != nullptr) {
		Stmt->Print(Spaces + 1);
	}
}

void Stmt_WHILE::Print(int Spaces) {
	print_indent(Spaces);
	cout << "while" << endl;
	Cond->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "do" << endl;
	if (Stmt != nullptr) {
		Stmt->Print(Spaces + 1);
	}
}

void Stmt_REPEAT::Print(int Spaces) {
	print_indent(Spaces);
	cout << "until" << endl;
	Cond->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "repeat" << endl;
	for (int i = 0; i < Stmt_List.size(); ++i){
		Stmt_List[i]->Print(Spaces + 1);
	}
}

void Stmt_BREAK::Print(int Spaces) {
	print_indent(Spaces);
	cout << "break" << endl;
}

void Stmt_Continue::Print(int Spaces) {
	print_indent(Spaces);
	cout << "continue" << endl;
}

void Stmt_Case::Print(int Spaces) {
	print_indent(Spaces);
	cout << "case" << endl;
	Exp->Print(Spaces);
	for (int i = 0; i < SelectorList.size(); ++i) {
		SelectorList[i].Stmt->Print(Spaces + 1);
	}
}

void Stmt_Try_Except::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try" << endl;
	for (int i = 0; i < Stmt_List_Try.size(); ++i){
		Stmt_List_Try[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "except" << endl;
	for (int i = 0; i < Stmt_List_Except.size(); ++i) {
		Stmt_List_Except[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "end" << endl;
}

void Stmt_Try_Finally::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try" << endl;
	for (int i = 0; i < Stmt_List_Try.size(); ++i) {
		Stmt_List_Try[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "finally" << endl;
	for (int i = 0; i < Stmt_List_Finally.size(); ++i) {
		Stmt_List_Finally[i]->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "end" << endl;
}

void Stmt_Raise::Print(int Spaces) {
	print_indent(Spaces);
	cout << "raise" << endl;
	Exp->Print(Spaces + 1);
}

void Stmt_Assign::Print(int Spaces) {
	Exp->Print(Spaces);
}