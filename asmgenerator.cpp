#include "asmgenerator.h"

static const string AsmOp_str[] = { "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov" };
static const string AsmRegistr_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

Asm_Cmd::Asm_Cmd(AsmOp Op) : Op(Op) {}
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOp Op, Asm_Operand* Oper1, Asm_Operand* Oper2): Asm_Cmd(Op), Oper1(Oper1), Oper2(Oper2) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOp Op, Asm_Operand* Oper1) : Asm_Cmd(Op), Oper1(Oper1) {}
Asm_Registr::Asm_Registr(AsmRegistr Reg) : Reg(Reg) {}
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

void Asm_Code::Add(AsmOp Op, AsmRegistr Reg) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Registr(Reg)));
}

void Asm_Code::Add(AsmOp Op, string Str) {
	if (Str[0] == '\'') {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_StringConst(Str)));
	}
	else {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_IntConst(Str)));
	}
}

void Asm_Code::Add(AsmOp Op, string var, int offset) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Address(var, offset)));
}

void Asm_Code::Add(AsmOp Op, AsmRegistr Reg1, AsmRegistr Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg1), new Asm_Registr(Reg2)));
}

void Asm_Code::Add(AsmOp Op, AsmRegistr Reg, string Str) {
	if (Str[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_StringConst(Str)));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_IntConst(Str)));
	}
}

void Asm_Code::Add(AsmOp Op, AsmRegistr Reg, string Var, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_Address(Var, offset)));
}

void Asm_Code::Add(AsmOp Op, string Str, AsmRegistr Reg) {
	if (Str[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_StringConst(Str), new Asm_Registr(Reg)));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_IntConst(Str), new Asm_Registr(Reg)));
	}
}

void Asm_Code::Add(AsmOp Op, string Var, int offset, AsmRegistr Reg) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Var, offset), new Asm_Registr(Reg)));
}

void Asm_Code::Print() {
	cout << "extern _printf" << endl;
	cout << "section .data" << endl;
	cout << "	base_str : times 256 db 0" << endl;
	cout << "	fmt : times 128 db 0" << endl;
	cout << "section .text" << endl;
	cout << "global _main" << endl;
	cout << "_main:" << endl;
	for (int i = 0; i < Cmds.size(); ++i) {
		cout << Cmds[i]->GetCode() << endl;
	}
	cout << "ret";
}