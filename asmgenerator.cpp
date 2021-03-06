#include "asmgenerator.h"

static const string AsmOp_str[] = { "", "push", "pop", "imul", "div", "add", "sub", "neg", "not", "or", "and", "xor", "shl", "shr", "call", "mov", "ret", "test", "cmp", "jz", 
	"jnz", "jmp", "jg", "jge", "jl", "jle", "je", "jne", "fld", "fild", "fstp", "fadd", "fsub", "fdiv", "fmul", "fiadd", "fisub", "fidiv", "fimul", "ja", "jb", "jae", "jbe",
	"fcom", "fcomi", "fcomip"
};
static const string AsmRegister_str[] =  { "eax", "ebx", "ecx", "edx", "ebp", "esp","ST0", "ST1" };
static const string AsmSize_str[] = { "qword", "dword" };

Asm_Cmd::Asm_Cmd(AsmOpType Op, int CmdIndex) : Op(Op), CmdIndex(CmdIndex) {}
Asm_Label::Asm_Label(string Name) : Name(Name), Asm_Cmd(Null, 0) {};
Asm_Bin_Cmd::Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2, int CmdIndex) : Asm_Cmd(Op, CmdIndex), Oper1(Oper1), Oper2(Oper2) {}
Asm_Bin_Left_Size_Cmd::Asm_Bin_Left_Size_Cmd(AsmOpType Op, AsmSize Size, Asm_Operand* Oper1, Asm_Operand* Oper2, int CmdIndex) : Asm_Bin_Cmd(Op, Oper1, Oper2, CmdIndex), Size(Size) {}
Asm_Unar_Cmd::Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1, int CmdIndex) : Asm_Cmd(Op, CmdIndex), Oper1(Oper1) {}
Asm_Unar_Size_Cmd::Asm_Unar_Size_Cmd(AsmOpType Op, AsmSize Size, Asm_Operand* Oper1, int CmdIndex) : Asm_Cmd(Op, CmdIndex), Size(Size), Oper1(Oper1) {}
Asm_Register::Asm_Register(AsmRegType Reg) : Reg(Reg) {}
Asm_StringConst::Asm_StringConst(string Str) : Str(Str) {}
Asm_IntConst::Asm_IntConst(string Val) : Val(Val) {}
Asm_Variable::Asm_Variable(string Val) : Val(Val) {}
Asm_Address::Asm_Address(string Val, int offset) : Oper(new Asm_Variable(Val)), offset(offset) {}
Asm_Address::Asm_Address(AsmRegType Reg, int offset) : Oper(new Asm_Register(Reg)), offset(offset) {}
Asm_Global_Data::Asm_Global_Data(string Name, string Type, string InitList) : Name(Name), Type(Type), InitList(InitList) {}
Asm_Local_Data::Asm_Local_Data(int depth, int size, int arg_size) : depth(depth), size(size), arg_size(arg_size) {}
Asm_Function::Asm_Function(string Name, vector<Asm_Cmd*> Cmds, int arg_size) : Name(Name), Cmds(Cmds), arg_size(arg_size){};
Asm_Code::Asm_Code() : Fmts(new vector<string>), ConstStrings(new vector<string>), depth(0), max_depth(0) {}

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

string Asm_Bin_Left_Size_Cmd::GetCode() {
	return AsmOp_str[Op] + " " + AsmSize_str[Size]  + " " + Oper1->GetCode() + ", " + Oper2->GetCode();
}

string Asm_Register::GetCode() {
	return AsmRegister_str[Reg];
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
	return "[" + Oper->GetCode() + (offset == 0 ? "" : " + " + to_string(offset)) + "]";
}

string Asm_Code::AddFormat(string new_format) {
	string num = to_string((*Fmts).size());
	(*Fmts).push_back("fmt_" + num + " : db " + new_format);
	return "fmt_" + num;
}

string Asm_Code::AddConstString(string new_string) {
	string num = to_string((*ConstStrings).size());
	(*ConstStrings).push_back("str_" + num + " : db \'" + new_string + "\', 0");
	return "str_" + num;
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
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Register(Reg), Cmd_Register));
}

void Asm_Code::Add(AsmOpType Op, string Val) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_StringConst(Val), Cmd_String));
	}
	else if (Val[0] >= '0' && Val[0] <= '9') {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_IntConst(Val), Cmd_Int));
	}
	else {
		Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Variable(Val), Cmd_Ident));
	}
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg1), new Asm_Register(Reg2), Cmd_Register_Register));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, string Val) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg), new Asm_StringConst(Val), Cmd_Register_String));
	}
	else if (Val[0] >= '0' && Val[0] <= '9'){
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg), new Asm_IntConst(Val), Cmd_Register_Int));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg), new Asm_Variable(Val), Cmd_Register_Ident));
	}
}

void Asm_Code::Add(AsmOpType Op, string Val, AsmRegType Reg) {
	if (Val[0] == '\'') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_StringConst(Val), new Asm_Register(Reg), Cmd_String_Register));
	}
	else if (Val[0] >= '0' && Val[0] <= '9') {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_IntConst(Val), new Asm_Register(Reg), Cmd_Int_Register));
	}
	else {
		Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Variable(Val), new Asm_Register(Reg), Cmd_Ident_Register));
	}
}

void Asm_Code::Add(string Name, string Type, string InitList) {
	Data.push_back(new Asm_Global_Data(Name, Type, InitList));
}

void Asm_Code::Add(Asm_Function* Func) {
	Functions.push_back(Func);
}

void Asm_Code::Add(AsmOpType Op, AsmSize Size, string Val) {
	Cmds.push_back(new Asm_Unar_Size_Cmd(Op, Size, new Asm_IntConst(Val), Cmd_Size_Int));
}

void Asm_Code::Add(AsmOpType Op, AsmSize Size, AsmAddr Addr, string Val, int offset) {
	Cmds.push_back(new Asm_Unar_Size_Cmd(Op, Size, new Asm_Address(Val, offset), Cmd_Size_Addr_Ident_Offset));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, int Value) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg), new Asm_IntConst(to_string(Value)), Cmd_Register_Int));
}

void Asm_Code::Add(AsmOpType Op) {
	Cmds.push_back(new Asm_Cmd(Op, Cmd));
}

void Asm_Code::Add(AsmOpType Op, AsmSize Size, AsmAddr Addr, AsmRegType Reg, int offset) {
	Cmds.push_back(new Asm_Unar_Size_Cmd(Op, Size, new Asm_Address(Reg, offset), Cmd_Size_Addr_Register_Offset));
}

void Asm_Code::Add(AsmOpType Op, AsmAddr Addr, string Var, int offset) {
	Cmds.push_back(new Asm_Unar_Cmd(Op, new Asm_Address(Var, offset), Cmd_Addr_Ident_Offset));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg, AsmAddr Addr, string Var, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg), new Asm_Address(Var, offset), Cmd_Register_Addr_Ident_Offset));
}

void Asm_Code::Add(AsmOpType Op, AsmAddr Addr, string Var, int offset, AsmRegType Reg) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Var, offset), new Asm_Register(Reg), Cmd_Addr_Ident_Offset_Register));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg1, AsmAddr Addr, AsmRegType Reg2, int offset) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg1), new Asm_Address(Reg2, offset), Cmd_Register_Addr_Register_Offset));
}

void Asm_Code::Add(AsmOpType Op, AsmAddr Addr, AsmRegType Reg1, int offset, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Reg1, offset), new Asm_Register(Reg2), Cmd_Addr_Register_Offset_Register));
}

void Asm_Code::Add(AsmOpType Op, AsmAddr Addr, AsmRegType Reg1, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Address(Reg1, 0), new Asm_Register(Reg2), Cmd_Addr_Register_Offset_Register));
}

void Asm_Code::Add(AsmOpType Op, AsmRegType Reg1, AsmAddr Addr, AsmRegType Reg2) {
	Cmds.push_back(new Asm_Bin_Cmd(Op, new Asm_Register(Reg1), new Asm_Address(Reg2, 0), Cmd_Register_Addr_Register_Offset));
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
	for (auto it = (*ConstStrings).begin(); it < (*ConstStrings).end(); ++it) {
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
