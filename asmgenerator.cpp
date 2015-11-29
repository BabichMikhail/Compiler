#include "asmgenerator.h"

static const string AsmOp_str[] = { "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov", "ret" };
static const string AsmRegistr_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

Asm_Cmd::Asm_Cmd(AsmOpType Op) : Op(Op) {}
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2) : Asm_Cmd(Op), Oper1(Oper1), Oper2(Oper2) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1) : Asm_Cmd(Op), Oper1(Oper1) {}
Asm_Registr::Asm_Registr(AsmRegType Reg) : Reg(Reg) {}
Asm_StringConst::Asm_StringConst(string Str) : Str(Str) {}
Asm_IntConst::Asm_IntConst(string Val) : Val(Val) {}
Asm_Variable::Asm_Variable(string Val) : Val(Val) {}
Asm_Address::Asm_Address(string Val, int offset) : Val(Val), offset(offset) {}
Asm_Address::Asm_Address(AsmRegType Reg, int offset) : Reg(Reg), offset(offset), Val("") {}
Asm_Global_Data::Asm_Global_Data(string Name, string Type, string InitList) : Name(Name), Type(Type), InitList(InitList) {}
Asm_Local_Data::Asm_Local_Data(int depth, int size, int arg_size) : depth(depth), size(size), arg_size(arg_size) {}
Asm_Function::Asm_Function(string Name, vector<Asm_Cmd*> Cmds, int arg_size) : Name(Name), Cmds(Cmds), arg_size(arg_size){};
Asm_Code::Asm_Code() : Fmts(new vector<string>) {}

string Asm_Unar_Cmd::GetCode() {
	return AsmOp_str[Op] + " " + Oper1->GetCode();
}

string Asm_Bin_Cmd::GetCode() {
	return AsmOp_str[Op] + " " + Oper1->GetCode() + ", " + Oper2->GetCode();
}

string Asm_Registr::GetCode() {
	return AsmRegistr_str[Reg];
}

string Asm_StringConst::GetCode() {
	return Str;
}

string Asm_IntConst::GetCode() {
	return Val;
}

string Asm_Variable::GetCode() {
	return Val;
}

string Asm_Address::GetCode() {
	return "[" + (Val == "" ? AsmRegistr_str[Reg] : Val) + (offset == 0 ? "" : " + " + to_string(offset)) + "]";
}

string Asm_Code::AddFormat(string new_format) {
	string num = to_string((*Fmts).size());
	(*Fmts).push_back("fmt_" + num + " : db " + new_format);
	return "fmt_" + num;
}

string Asm_Global_Data::GetCode() {
	return Name + " : " + Type + " " + InitList;
}

string Asm_Function::GetCode() {
	string Ans;
	Ans = Name + ":\n";
	for (int i = 0; i < Cmds.size(); ++i) {
		Ans += "    " + Cmds[i]->GetCode() + "\n";
	}
	
	for (int i = 0; i < this->Functions.size(); ++i) {
		Ans += Functions[i]->GetCode() + "\n";
	}
	return Ans;
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Registr(Reg)));
}

void Asm_Code::Add(AsmOpType Op, string Val) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_StringConst(Val)));
	}
	else if (Val[0] >= '0' && Val[0] <= '9') {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_IntConst(Val)));
	}
	else {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Variable(Val)));
	}
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg1), new Asm_Registr(Reg2)));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, string Val) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_StringConst(Val)));
	}
	else if (Val[0] >= '0' && Val[0] <= '9'){
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_IntConst(Val)));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_Variable(Val)));
	}
}

void Asm_Code::Add(AsmOpType Op, string Val, AsmRegType Reg) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_StringConst(Val), new Asm_Registr(Reg)));
	}
	else if (Val[0] >= '0' && Val[0] <= '9') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_IntConst(Val), new Asm_Registr(Reg)));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Variable(Val), new Asm_Registr(Reg)));
	}
}

void Asm_Code::Add(string Name, string Type, string InitList) {
	Data.push_back(new Asm_Global_Data(Name, Type, InitList));
}

void Asm_Code::Add(Asm_Function* Func) {
	Functions.push_back(Func);
}

void Asm_Code::Add_Addr(AsmOpType Op, string Var, int offset) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Address(Var, offset)));
}

void Asm_Code::Add_RAddr(AsmOpType Op, AsmRegType Reg, string Var, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_Address(Var, offset)));
}

void Asm_Code::Add_LAddr(AsmOpType Op, string Var, int offset, AsmRegType Reg) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Var, offset), new Asm_Registr(Reg)));
}

void Asm_Code::Add_RAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg1), new Asm_Address(Reg2, offset)));
}

void Asm_Code::Add_LAddr(AsmOpType Op, AsmRegType Reg1, int offset, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Reg1, offset), new Asm_Registr(Reg2)));
}

void Asm_Code::Add_LAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Reg1, 0), new Asm_Registr(Reg2)));
}

void Asm_Code::Add_RAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg1), new Asm_Address(Reg2, 0)));
}

void Asm_Code::Print() {
	cout << "extern _printf" << endl;
	cout << "section .data" << endl;
	cout << "    base_str : times 256 db 0" << endl;
	for (int i = 0; i < (*Fmts).size(); ++i) {
		cout << "    " + (*Fmts)[i] << endl;
	}
	for (int i = 0; i < Data.size(); ++i) {
		cout << "    " + Data[i]->GetCode() << endl;
	}
	cout << "section .text" << endl;
	for (int i = 0; i < Functions.size(); ++i) {
		cout << Functions[i]->GetCode() << endl;
	}
	cout << "global _main" << endl;
	cout << "_main:" << endl;
	for (int i = 0; i < Cmds.size(); ++i) {
		cout << "    " + Cmds[i]->GetCode() << endl;
	}
	cout << "ret";
}