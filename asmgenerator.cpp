#include "asmgenerator.h"

static const string AsmOp_str[] = { "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov" };
static const string AsmRegistr_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

Asm_Cmd::Asm_Cmd(AsmOpType Op) : Op(Op) {}
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2) : Asm_Cmd(Op), Oper1(Oper1), Oper2(Oper2) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1) : Asm_Cmd(Op), Oper1(Oper1) {}
Asm_Registr::Asm_Registr(AsmRegType Reg) : Reg(Reg) {}
Asm_StringConst::Asm_StringConst(string Str) : Str(Str) {}
Asm_IntConst::Asm_IntConst(string Val) : Val(Val) {}
Asm_Variable::Asm_Variable(string Val) : Val(Val) {}
Asm_Address::Asm_Address(string Val, int offset) : Val(Val), offset(offset) {}

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
	return "[" + Val + " + " + to_string(offset) + "]";
}

string Asm_Code::AddFormat(string new_format) {
	string num = to_string(Fmts.size());
	Fmts.push_back("fmt_" + num + " : db " + new_format);
	return "fmt_" + num;
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Registr(Reg)));
}

void Asm_Code::Add(AsmOpType Op, string Str) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, Str[0] == '\'' ? new Asm_StringConst(Str) : (Asm_Operand*)new Asm_IntConst(Str)));
}

void Asm_Code::Add(AsmOpType Op, string var, int offset) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Address(var, offset)));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg1), new Asm_Registr(Reg2)));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, string Str) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), Str[0] == '\'' ? new Asm_StringConst(Str) : (Asm_Operand*)new Asm_IntConst(Str)));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, string Var, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_Address(Var, offset)));
}

void Asm_Code::Add(AsmOpType Op, string Str, AsmRegType Reg) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, Str[0] == '\'' ? new Asm_StringConst(Str) : (Asm_Operand*)new Asm_IntConst(Str), new Asm_Registr(Reg)));
}

void Asm_Code::Add(AsmOpType Op, string Var, int offset, AsmRegType Reg) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Var, offset), new Asm_Registr(Reg)));
}

void Asm_Code::Print() {
	cout << "extern _printf" << endl;
	cout << "section .data" << endl;
	cout << "	base_str : times 256 db 0" << endl;
	for (int i = 0; i < Fmts.size(); ++i) {
		cout << "	" + Fmts[i] << endl;
	}
	cout << "section .text" << endl;
	cout << "global _main" << endl;
	cout << "_main:" << endl;
	for (int i = 0; i < Cmds.size(); ++i) {
		cout << "	" + Cmds[i]->GetCode() << endl;
	}
	cout << "ret";
}