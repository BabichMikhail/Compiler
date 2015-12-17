#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include <string>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

enum AsmOpType { Null = 0, Push, Pop, IMul, Div, Add, Sub, Neg, Not, Or, And, Xor, Shl, Shr, Call, Mov, Ret, Test, Cmp, Jz, Jnz, Jmp, Jg, Jge, Jl, Jle, Je, Jne, 
	Fld, Fild, Fstp, Fadd, Fsub, Fdiv, Fmul, Fiadd, Fisub, Fidiv, Fimul, Ja, Jb, Jae, Jbe, Fcom, Fcomi, Fcomip };
enum AsmRegType { EAX = 0, EBX, ECX, EDX, EBP, ESP, ST0, ST1 };
enum AsmSize { qword, dword };
enum AsmAddr { addr };
enum AsmCmdIndex { 
	Label, Cmd_Register, Cmd_Size_Addr_Register_Offset, Cmd_String, Cmd_Register_Int, Cmd_Register_Ident, Cmd_Addr_Register_Register, Cmd_Int, Cmd_Ident, Cmd_Label, 
	Cmd_Register_Register, Cmd_Register_Addr_Ident_Offset, Cmd_Register_String, Cmd_String_Register, Cmd_Int_Register, Cmd_Ident_Register, Cmd_Size_Addr_Ident_Offset, 
	Cmd, Cmd_Addr_Ident_Offset, Cmd_Addr_Ident_Offset_Register, Cmd_Register_Addr_Register_Offset, Cmd_Addr_Register_Offset_Register, Cmd_Size_Int, Cmd_Size_Addr_Register_Register
};

class Asm_Operand {
public:
	virtual string GetCode() = 0;
};

class Asm_Register : public Asm_Operand{
public:
	AsmRegType Reg;
	string GetCode();
	Asm_Register(AsmRegType Reg);
};

class Asm_StringConst : public Asm_Operand {
public:
	string Str;
	string GetCode();
	Asm_StringConst(string Str);
};

class Asm_IntConst : public Asm_Operand{
public:
	string Val;
	string GetCode();
	Asm_IntConst(string Val);
};

class Asm_Variable : public Asm_Operand{
public:
	string Val; 
	string GetCode();
	Asm_Variable(string Val);
};

class Asm_Address : public Asm_Operand {
public:
	Asm_Operand* Oper;
	int offset;
	string GetCode();
	Asm_Address(string Val, int offset);
	Asm_Address(AsmRegType Reg, int offset);
};

class Asm_Cmd {
public:
	AsmOpType Op;
	int CmdIndex;
	virtual string GetCode();
	Asm_Cmd(AsmOpType Cmd, int CmdIndex);
};

class Asm_Label : public Asm_Cmd {
public:
	string Name;
	Asm_Label(string Name);
	string GetCode();
};

class Asm_Bin_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	Asm_Operand* Oper2;
	string GetCode();
	Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2, int CmdIndex);
};

class Asm_Bin_Left_Size_Cmd : public Asm_Bin_Cmd{
public:
	AsmSize Size;
	string GetCode();
	Asm_Bin_Left_Size_Cmd(AsmOpType Op, AsmSize Size, Asm_Operand* Oper1, Asm_Operand* Oper2, int CmdIndex);
};

class Asm_Unar_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	string GetCode();
	Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1, int CmdIndex);
};

class Asm_Unar_Size_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	AsmSize Size;
	string GetCode();
	Asm_Unar_Size_Cmd(AsmOpType Op, AsmSize Size, Asm_Operand* Oper1, int CmdIndex);
};

class Asm_Global_Data {
public:
	string Name;
	string Type;
	string InitList;
	string GetCode();
	Asm_Global_Data(string Name, string Type, string InitList);
};

class Asm_Local_Data {
public:
	int depth; 
	int size;
	int arg_size;
	Asm_Local_Data(int depth, int size, int arg_size);
};

class Asm_Function {
public:
	int arg_size;
	string Name;
	vector<Asm_Cmd*> Cmds;
	vector<Asm_Function*> Functions;
	string GetCode();
	Asm_Function(string Name, vector<Asm_Cmd*> Cmds, int arg_size);
};

class Asm_Code {
private:
	vector<Asm_Global_Data*> Data;
	int Label_Count, Double_Count, Error_Count, Global_Label_Count;
	vector<string> BreakLabelNames;
	vector<string> ContinueLabelNames;
public:
	int depth, max_depth;
	vector<Asm_Cmd*> Cmds; 
	vector<string> *Fmts, *ConstStrings;
	vector<Asm_Function*> Functions;
	string AddFormat(string new_format);
	string AddConstString(string new_string);
	void AddLabel(string Name);
	string GetNewErrorLabelName();
	string ErrroLabelName;
	string GetLocalLabelName();
	string GetGlobalLabelName(string Name);
	string GetGlobalLabelName();
	string AddDoubleVar(string Value);
	void Add(AsmOpType Op, AsmRegType Reg);
	void Add(AsmOpType Op, string Val);
	void Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2);
	void Add(AsmOpType Op, AsmRegType Reg, string Val);
	void Add(AsmOpType Op, string Val, AsmRegType Reg);
	void Add(AsmOpType Op, AsmRegType Reg, int Value);
	void Add(string Name, string Type, string InitList); 
	void Add(Asm_Function* Func);
	void Add(AsmOpType Op, AsmSize Size, string Val);
	void Add(AsmOpType Op, AsmSize Size, AsmAddr Addr, string Val, int offset = 0);
	void Add(AsmOpType Op);
	void Add(AsmOpType Op, AsmSize Size, AsmAddr Addr, AsmRegType Reg, int offset = 0);
	void Add(AsmOpType Op, AsmAddr Addr, string Var, int offset);
	void Add(AsmOpType Op, AsmRegType Reg, AsmAddr Addr, string Var, int offset);
	void Add(AsmOpType Op, AsmAddr Addr, string Val, int offset, AsmRegType Reg);
	void Add(AsmOpType Op, AsmRegType Reg1, AsmAddr Addr, AsmRegType Reg2, int offset);
	void Add(AsmOpType Op, AsmAddr Addr, AsmRegType Reg1, int offset, AsmRegType Reg2);
	void Add(AsmOpType Op, AsmAddr Addr, AsmRegType Reg1, AsmRegType Reg2);
	void Add(AsmOpType Op, AsmRegType Reg1, AsmAddr Addr, AsmRegType Reg2);
	void Print();
	Asm_Code();
	void SaveLabels(string LabelContinue, string LabelBreak);
	void LoadLabels();
	string GetLabelContinue();
	string GetLabelBreak();
};

#endif