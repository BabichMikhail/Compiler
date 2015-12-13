#include "statement.h"
#include "symtable.h"

Stmt_Goto::Stmt_Goto(Symbol* Sym) : LabelSym(Sym){};
Stmt_Goto_Label::Stmt_Goto_Label(Symbol* Sym) : LabelSym(Sym) {};
Stmt_Conditional::Stmt_Conditional(Expr* Exp) : Exp(Exp){};
Stmt_If::Stmt_If(Expr* Exp, Statement* Stmt, Statement* StmtElse) : Stmt_Conditional(Exp), Stmt(Stmt), StmtElse(StmtElse){}

Stmt_For::Stmt_For(Expr* Exp_1, Expr* Exp_2, bool isTO, Statement* Stmt) : Exp_1(Exp_1), Exp_2(Exp_2), isTO(isTO), Stmt(Stmt),
	Stmt_Conditional(new ExprBinOp(((ExprAssign*)Exp_1)->Left, Token(Position(), isTO ? ">" : "<", isTO ? TK_GREAT : TK_LESS), Exp_2)) {};
Stmt_While::Stmt_While(Expr* Cond, Statement* Stmt) : Stmt_Conditional(Cond), Stmt(Stmt) {};
Stmt_Repeat::Stmt_Repeat(Expr* Cond, vector<Statement*> StmtList) : Stmt_Conditional(Cond), StmtList(StmtList) {};

Case_Selector::Case_Selector(Expr* Exp_1, Expr* Exp_2, Statement* Stmt) : Exp_1(Exp_1), Exp_2(Exp_2), Stmt(Stmt) {};
Stmt_Case::Stmt_Case(Expr* Exp) : Stmt_Conditional(Exp), StmtElse(nullptr) {};

Stmt_Try_Except::Stmt_Try_Except(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Except) : Stmt_List_Try(Stmt_Try), Stmt_List_Except(Stmt_Except){}
Stmt_Try_Finally::Stmt_Try_Finally(vector<Statement*> Stmt_Try, vector<Statement*> Stmt_Finally) : Stmt_List_Try(Stmt_Try), Stmt_List_Finally(Stmt_Finally){}

Stmt_Raise::Stmt_Raise(Expr* Exp) : Exp(Exp) {};

Stmt_Assign::Stmt_Assign(Expr* Exp) : Exp(Exp) {};
Stmt_Call::Stmt_Call(Expr* Exp) : Exp(Exp) {}

void Stmt_Compound::Add(Statement* Stmt){
	StmtList.push_back(Stmt);
}

void Stmt_Case::Add(Case_Selector Selector) {
	SelectorList.push_back(Selector);
}

void Stmt_Compound::Print(int Spaces) {
	print_indent(Spaces);
	cout << "begin" << endl;
	for (auto it = StmtList.begin(); it < StmtList.end(); ++it) {
		(*it)->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "end" << endl;
}

void Stmt_Goto::Print(int Spaces) {
	print_indent(Spaces);
	cout << "goto" << indent << LabelSym->Name << endl;
}

void Stmt_Goto_Label::Print(int Spaces) {
	print_indent(Spaces);
	cout << "goto_label" << indent << LabelSym->Name << endl;
}

void Stmt_If::Print(int Spaces){
	print_indent(Spaces);
	cout << "if" << endl;
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "then" << endl;
	Stmt->Print(Spaces + 1);
	if (StmtElse != nullptr) {
		print_indent(Spaces);
		cout << "else" << endl;
		StmtElse->Print(Spaces + 1);
	}
}

void Stmt_For::Print(int Spaces){
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

void Stmt_While::Print(int Spaces) {
	print_indent(Spaces);
	cout << "while" << endl;
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "do" << endl;
	if (Stmt != nullptr) {
		Stmt->Print(Spaces + 1);
	}
}

void Stmt_Repeat::Print(int Spaces) {
	print_indent(Spaces);
	cout << "until" << endl;
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "repeat" << endl;
	for (auto it = StmtList.begin(); it < StmtList.end(); ++it){
		(*it)->Print(Spaces + 1);
	}
}

void Stmt_Break::Print(int Spaces) {
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
	for (auto it = SelectorList.begin(); it < SelectorList.end(); ++it) {
		(*it).Stmt->Print(Spaces + 1);
	}
}

void Stmt_Try_Except::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try" << endl;
	for (auto it = Stmt_List_Try.begin(); it < Stmt_List_Try.end(); ++it){
		(*it)->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "except" << endl;
	for (auto it = Stmt_List_Except.begin(); it < Stmt_List_Except.end(); ++it) {
		(*it)->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "end" << endl;
}

void Stmt_Try_Finally::Print(int Spaces) {
	print_indent(Spaces);
	cout << "try" << endl;
	for (auto it = Stmt_List_Try.begin(); it < Stmt_List_Try.end(); ++it) {
		(*it)->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "finally" << endl;
	for (auto it = Stmt_List_Finally.begin(); it < Stmt_List_Finally.end(); ++it) {
		(*it)->Print(Spaces + 1);
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

void Stmt_Call::Print(int Spaces) {
	print_indent(Spaces);
	cout << "Call" << endl;
	Exp->Print(Spaces + 1);
}

void Stmt_Goto::Generate(Asm_Code* Code) {
	Code->Add(Jmp, Code->GetGlobalLabelName(LabelSym->Name));
}

void Stmt_Goto_Label::Generate(Asm_Code* Code) {
	Code->AddLabel(Code->GetGlobalLabelName(LabelSym->Name));
}

void Stmt_Compound::Generate(Asm_Code* Code) {
	for (auto it = StmtList.begin(); it < StmtList.end(); ++it) {
		(*it)->Generate(Code);
	}
}

void Stmt_Conditional::Generate_Condition(Asm_Code* Code) {
	Exp->Generate(Code);
	Code->Add(Pop, EAX);
	Code->Add(Test, EAX, EAX);
}

void Stmt_If::Generate(Asm_Code* Code) {
	string LabelName_Else = Code->GetLocalLabelName();
	Generate_Condition(Code);
	Code->Add(Jz, LabelName_Else);
	Stmt->Generate(Code);
	Code->AddLabel(LabelName_Else);
	if (StmtElse != nullptr) {
		StmtElse->Generate(Code);
	}
}

void Stmt_Case::Generate(Asm_Code* Code) {
	string LabelName_Break = Code->GetLocalLabelName();
	for (auto it = SelectorList.begin(); it < SelectorList.end(); ++it) {
		if (it->Exp_2 == nullptr) {
			ExprBinOp(Exp, Token(TK_EQUAL), it->Exp_1).Generate(Code);
			Code->Add(Pop, EAX);
		}
		else {
			ExprBinOp(Exp, Token(TK_GREAT_EQUAL), it->Exp_1).Generate(Code);
			ExprBinOp(Exp, Token(TK_LESS_EQUAL), it->Exp_2).Generate(Code);
			Code->Add(Pop, EAX);
			Code->Add(Pop, EBX);
			Code->Add(And, EAX, EBX);			
		}
		Code->Add(Test, EAX, EAX);
		string LabelName_Else = Code->GetLocalLabelName();
		Code->Add(Jz, LabelName_Else);
		it->Stmt->Generate(Code);
		Code->Add(Jmp, LabelName_Break);
		Code->AddLabel(LabelName_Else);
	}
	if (StmtElse != nullptr) {
		StmtElse->Generate(Code);
	}
	Code->AddLabel(LabelName_Break);
}

void Stmt_For::Generate(Asm_Code* Code) {
	Exp_1->Generate(Code);
	string LabelName_Cond = Code->GetLocalLabelName();
	string LabelName_Break = Code->GetLocalLabelName();
	Code->SaveLabels(LabelName_Cond, LabelName_Break);
	Code->AddLabel(LabelName_Cond);
	Generate_Condition(Code);
	Code->Add(Jnz, LabelName_Break);
	Stmt->Generate(Code);
	ExprAssign(((ExprAssign*)Exp_1)->Left, new ExprBinOp(((ExprAssign*)Exp_1)->Left, Token(isTO ? TK_PLUS : TK_MINUS), new ExprIntConst("1"))).Generate(Code);
	Code->Add(Jmp, LabelName_Cond);
	Code->AddLabel(LabelName_Break);
	Code->LoadLabels();
}

void Stmt_While::Generate(Asm_Code* Code) {
	string LabelName_Cond = Code->GetLocalLabelName();
	string LabelName_Break = Code->GetLocalLabelName();
	Code->SaveLabels(LabelName_Cond, LabelName_Break);
	Code->AddLabel(LabelName_Cond);
	Generate_Condition(Code);
	Code->Add(Jz, LabelName_Break);
	Stmt->Generate(Code);
	Code->Add(Jmp, LabelName_Cond);
	Code->AddLabel(LabelName_Break);
	Code->LoadLabels();
}

void Stmt_Repeat::Generate(Asm_Code* Code) {
	string LabelName_Body = Code->GetLocalLabelName();
	string LabelName_Break = Code->GetLocalLabelName();
	string LabelName_Cond = Code->GetLocalLabelName();
	Code->SaveLabels(LabelName_Cond, LabelName_Break);
	Code->AddLabel(LabelName_Body);
	for (auto it = StmtList.begin(); it < StmtList.end(); ++it) {
		(*it)->Generate(Code);
	}
	Code->AddLabel(LabelName_Cond);
	Generate_Condition(Code); 
	Code->Add(Jz, LabelName_Body);
	Code->AddLabel(LabelName_Break);
	Code->LoadLabels();
}

void Stmt_Break::Generate(Asm_Code* Code) {
	Code->Add(Jmp, Code->GetLabelBreak());
}

void Stmt_Continue::Generate(Asm_Code* Code) {
	Code->Add(Jmp, Code->GetLabelContinue());
}

void Stmt_Try_Except::Generate(Asm_Code* Code) {
	string OldErrorLabelName = Code->ErrroLabelName;
	string LabelAfterExcept = Code->GetGlobalLabelName();
	Code->ErrroLabelName = Code->GetNewErrorLabelName();
	for (auto it = Stmt_List_Try.begin(); it < Stmt_List_Try.end(); ++it) {
		(*it)->Generate(Code);
	}
	Code->Add(Jmp, LabelAfterExcept);
	Code->AddLabel(Code->ErrroLabelName);
	for (auto it = Stmt_List_Except.begin(); it < Stmt_List_Except.end(); ++it) {
		(*it)->Generate(Code);
	}
	Code->AddLabel(LabelAfterExcept);
}

void Stmt_Try_Finally::Generate(Asm_Code* Code) {
	string OldErrorLabelName = Code->ErrroLabelName;
	Code->ErrroLabelName = Code->GetNewErrorLabelName();
	for (auto it = Stmt_List_Try.begin(); it < Stmt_List_Try.end(); ++it) {
		(*it)->Generate(Code);
	}
	Code->AddLabel(Code->ErrroLabelName);
	for (auto it = Stmt_List_Finally.begin(); it < Stmt_List_Finally.end(); ++it) {
		(*it)->Generate(Code);
	}
}

void Stmt_Raise::Generate(Asm_Code* Code) {
	string FormatName = Code->AddFormat("\'%s\', 0xA, 0x0");
	Exp->Generate(Code);
	Code->Add(Push, FormatName);
	Code->Add(Call, "_printf");
	Code->Add(Add, ESP, 8);
	Code->Add(Jmp, Code->ErrroLabelName);
}

void Stmt_Assign::Generate(Asm_Code* Code) {
	Exp->Generate(Code);
}

void Stmt_Call::Generate(Asm_Code* Code) {
	if (((ExprIdent*)((ExprFunction*)Exp)->Left)->Sym->Section == DeclFunction) {
		Exp->Generate(Code);
		SymFunction* Sym = (SymFunction*)((ExprIdent*)((ExprFunction*)Exp)->Left)->Sym;
		Code->Add(Add, ESP, ((SymIdent*)Sym->Table->Symbols[Sym->argc - 1])->Type->GetSize());
	}
	else {
		Exp->Generate(Code);
	}
}