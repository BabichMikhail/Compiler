#include "asmgenerator.h"

static const string AsmOp_str[] = { "", "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov", "ret", "test", "cmp", "jz", 
	"jnz", "jmp", "jg", "jge", "jl", "jle", "je", "jne", "fld", "fild", "fstp", "fadd", "fsub", "fdiv", "fmul", "fiadd", "fisub", "fidiv", "fimul", "ja", "jb", "jae", "jbe",
	"fcom", "fcomi", "fcomip"
};
static const string AsmRegistr_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp","ST0", "ST1" };
static const string AsmSize_str[] = { "qword", "dword" };

Asm_Cmd::Asm_Cmd(AsmOpType Op) : Op(Op) {}
Asm_Label::Asm_Label(string Name) : Name(Name), Asm_Cmd(Null) {};
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2) : Asm_Cmd(Op), Oper1(Oper1), Oper2(Oper2) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1) : Asm_Cmd(Op), Oper1(Oper1) {}
Asm_Unar_Size_Cmd::Asm_Unar_Size_Cmd(AsmOpType Op, AsmSize Size, Asm_Operand* Oper1) : Asm_Cmd(Op), Size(Size), Oper1(Oper1) {}
Asm_Registr::Asm_Registr(AsmRegType Reg) : Reg(Reg) {}
Asm_StringConst::Asm_StringConst(string Str) : Str(Str) {}
Asm_IntConst::Asm_IntConst(string Val) : Val(Val) {}
Asm_Variable::Asm_Variable(string Val) : Val(Val) {}
Asm_Address::Asm_Address(string Val, int offset) : Val(Val), offset(offset) {}
Asm_Address::Asm_Address(AsmRegType Reg, int offset) : Reg(Reg), offset(offset), Val("") {}
Asm_Global_Data::Asm_Global_Data(string Name, string Type, string InitList) : Name(Name), Type(Type), InitList(InitList) {}
Asm_Local_Data::Asm_Local_Data(int depth, int size, int arg_size) : depth(depth), size(size), arg_size(arg_size) {}
Asm_Function::Asm_Function(string Name, vector<Asm_Cmd*> Cmds, int arg_size) : Name(Name), Cmds(Cmds), arg_size(arg_size){};
Asm_Code::Asm_Code() : Fmts(new vector<string>), depth(0), max_depth(0) {}

string Asm_Cmd::GetCode() {
	return AsmOp_str[Op];
}

string Asm_Label::GetCode() {
	return Name + ":";
}

string Asm_Unar_Cmd::GetCode() {
	return AsmOp_str[Op] + " " + Oper1->GetCode();
}

string Asm_Unar_Size_Cmd::GetCode() {
	return AsmOp_str[Op] + " " + AsmSize_str[Size] + " " + Oper1->GetCode();
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

void Asm_Code::Add(AsmOpType Op, AsmSize Size, string Val) {
	Cmds.push_back(new Asm_Unar_Size_Cmd(Op, Size, new Asm_IntConst(Val)));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, int Value) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Registr(Reg), new Asm_IntConst(to_string(Value))));
}

void Asm_Code::Add(AsmOpType Op) {
	Cmds.push_back(new Asm_Cmd(Op));
}

void Asm_Code::Add_Addr(AsmOpType Op, AsmSize Size, AsmRegType Reg, int offset) {
	Cmds.push_back(new Asm_Unar_Size_Cmd(Op, Size, new Asm_Address(Reg, offset)));
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

void Asm_Code::AddLabel(string Name) {
	Cmds.push_back(new Asm_Label(Name));
}

string Asm_Code::GetNewErrorLabelName() {
	return "E_" + to_string(++Error_Count);
}

string Asm_Code::GetGlobalLabelName(string Name) {
	return "L_" + Name;
}

string Asm_Code::GetGlobalLabelName() {
	return "G_" + to_string(Global_Label_Count);
}

string Asm_Code::GetLocalLabelName() {
	return ".L_" + to_string(++Label_Count);
}

string Asm_Code::AddDoubleVar(string Value) {
	++Double_Count;
	string Name = "D_" + to_string(Double_Count);
	Add(Name, "dq", Value);
	return Name;
}

void Asm_Code::Print() {
	cout << "extern _printf" << endl;
	cout << "section .data" << endl;
	cout << "    depth : times " + to_string(max_depth*4) + " dd 0" << endl; 
	cout << "    base_str : times 256 db 0" << endl;
	for (auto it = (*Fmts).begin(); it < (*Fmts).end(); ++it) {
		cout << "    " + *it << endl;
	}
	for (auto it = Data.begin(); it < Data.end(); ++it) {
		cout << "    " + (*it)->GetCode() << endl;
	}
	cout << "section .text" << endl;
	for (auto it = Functions.begin(); it < Functions.end(); ++it) {
		cout << (*it)->GetCode() << endl;
	}
	cout << "global _main" << endl;
	cout << "_main:" << endl;
	for (auto it = Cmds.begin(); it < Cmds.end(); ++it) {
		cout << "    " + (*it)->GetCode() << endl;
	}
	cout << "    ret";
}

void Asm_Code::SaveLabels(string LabelContinue, string LabelBreak) {
	BreakLabelNames.push_back(LabelBreak);
	ContinueLabelNames.push_back(LabelContinue);
}

void Asm_Code::LoadLabels() {
	BreakLabelNames.pop_back();
	ContinueLabelNames.pop_back();
}

string Asm_Code::GetLabelBreak() {
	return *BreakLabelNames.crbegin();
}

string Asm_Code::GetLabelContinue() {
	return *ContinueLabelNames.crbegin();
}
