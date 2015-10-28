#include "statement.h"

#define indent "   "
#define print_indent(spaces) for (int i = 0; i < spaces; ++i) cout << indent

Stmt_GOTO::Stmt_GOTO(Symbol* Sym) : LabelSym(Sym){};
Stmt_Conditional::Stmt_Conditional(Expr* Exp) : Exp(Exp){};
Stmt_IF::Stmt_IF(Expr* Exp, Statement* Stmt, Statement* Stmt_Else) : Stmt_Conditional(Exp), Stmt(Stmt), Stmt_Else(Stmt_Else){}

Stmt_FOR::Stmt_FOR(Expr* Exp_1, Expr* Exp_2, bool isTO, Statement* Stmt) : Exp_1(Exp_1), Exp_2(Exp_2), isTO(isTO), Stmt(Stmt){};
Stmt_WHILE::Stmt_WHILE(Expr* Cond, Statement* Stmt) : Cond(Cond), Stmt(Stmt) {};
Stmt_REPEAT::Stmt_REPEAT(Expr* Cond, Statement* Stmt) : Cond(Cond), Stmt(Stmt) {};

Case_Selector::Case_Selector(Expr* Exp_1, Expr* Exp_2, Statement* Stmt0) : Exp_1(Exp_1), Exp_2(Exp_2), Stmt(Stmt) {};
Stmt_Case::Stmt_Case(Expr* Exp) : Stmt_Conditional(Exp), Stmt_Else(nullptr) {};

Stmt_Try_Except::Stmt_Try_Except(Statement* Stmt_1, Statement* Stmt_2) : Stmt_1(Stmt_1), Stmt_2(Stmt_2){}
Stmt_Try_Finally::Stmt_Try_Finally(Statement* Stmt_1, Statement* Stmt_2) : Stmt_1(Stmt_1), Stmt_2(Stmt_2){}

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
	cout << "end";
}

void Stmt_GOTO::Print(int Spaces) {
	print_indent(Spaces);
	cout << "goto" << endl;
	LabelSym->Print();
}

void Stmt_IF::Print(int Spaces){
	print_indent(Spaces);
	cout << "if" << endl;
	Exp->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
	Stmt_Else->Print(Spaces + 1);
}

void Stmt_FOR::Print(int Spaces){
	print_indent(Spaces);
	cout << "for" << endl;
	Exp_1->Print(Spaces + 1);
	print_indent(Spaces + 1);
	isTO ? cout << "true" << endl : cout << "false" << endl;
	Exp_2->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
}

void Stmt_WHILE::Print(int Spaces) {
	print_indent(Spaces);
	cout << "while" << endl;
	Cond->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
}

void Stmt_REPEAT::Print(int Spaces) {
	print_indent(Spaces);
	cout << "repeat" << endl;
	Cond->Print(Spaces + 1);
	Stmt->Print(Spaces + 1);
}

void Stmt_BREAK::Print(int Spaces) {
	print_indent(Spaces);
	cout << "break" << endl;
}

void Stmt_Case::Print(int Spaces) {
	print_indent(Spaces);
	cout << "case" << endl;
	Exp->Print(Spaces);
	for (int i = 0; i < SelectorList.size(); ++i) {
		// const Expr!! 
		SelectorList[i].Stmt->Print(Spaces + 1);
	}
}

void Stmt_Try_Except::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try-except" << endl;
	Stmt_1->Print(Spaces + 1);
	Stmt_2->Print(Spaces + 1);
}

void Stmt_Try_Finally::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try-finally" << endl;
	Stmt_1->Print(Spaces + 1);
	Stmt_2->Print(Spaces + 1);
}
