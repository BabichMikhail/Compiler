#include "asmgenerator.h"

static const string AsmOp_str[] = { "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov" };
static const string AsmRegistr_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp" };

Asm_Cmd::Asm_Cmd(AsmOp Cmd) : Cmd(Cmd) {}
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOp Cmd, Asm_Code* Oper1, Asm_Code* Oper2): Asm_Cmd(Cmd), Oper1(Oper1), Oper2(Oper2) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOp Cmd, Asm_Code* Oper1) : Asm_Cmd(Cmd), Oper1(Oper1) {}
Asm_Registr::Asm_Registr(AsmRegistr Reg) : Reg(Reg) {}; 
Asm_IntConst::Asm_IntConst(string Val) : Val(Val) {};
Asm_Variable::Asm_Variable(string Val) : Val(Val) {};
Asm_Address::Asm_Address(string Val, int offset) : Val(Val), offset(offset) {};

string Asm_Unar_Cmd::GetCode() {
	return AsmOp_str[Cmd] + " " + Oper1->GetCode() + "\n";
}

string Asm_Bin_Cmd::GetCode() {
	return AsmOp_str[Cmd] + " " + Oper1->GetCode() + ", " + Oper2->GetCode() + "\n";
}

string Asm_Registr::GetCode() {
	return AsmRegistr_str[Reg];
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

void Asm_Generator::SetCode() {
	Code = CodeParser->Stmt->GetAsmCode();
}

void Asm_Generator::CodeToStdout() {
	cout << "extern _printf" << endl;
	cout << "section .data" << endl;
	cout << "	fmt : db '%d', 0xA, 0" << endl;
	cout << "section .text" << endl;
	cout << "global _main" << endl;
	cout << "_main:" << endl;
	for (int i = 0; i < Code.size(); ++i) {
		cout << Code[i]->GetCode();
	}
	cout << "ret";
}

Asm_Generator::Asm_Generator(const char* filename, PMod State) : CodeParser(new Parser(filename, State)) {}