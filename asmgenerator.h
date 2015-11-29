#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum AsmOpType { Push = 0, Pop, IMul, Div, Add, Sub, Neg, Not, Or, And, Xor, Shl, Shr, Call, Mov, Ret };
enum AsmRegType { EAX = 0, EBX, ECX, EDX, EBP, ESP };

class Asm_Operand {
public:
	virtual string GetCode() = 0;
};

class Asm_Registr : public Asm_Operand{
public:
	AsmRegType Reg;
	string GetCode();
	Asm_Registr(AsmRegType Reg);
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
	string Val;
	AsmRegType Reg;
	int offset;
	string GetCode();
	Asm_Address(string Val, int offset);
	Asm_Address(AsmRegType Reg, int offset);
};

class Asm_Cmd {
public:
	AsmOpType Op;
	virtual string GetCode() = 0;
	Asm_Cmd(AsmOpType Cmd);
};

class Asm_Bin_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	Asm_Operand* Oper2;
	string GetCode();
	Asm_Bin_Cmd(AsmOpType Op, Asm_Operand* Oper1, Asm_Operand* Oper2);
};

class Asm_Unar_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	string GetCode();
	Asm_Unar_Cmd(AsmOpType Op, Asm_Operand* Oper1);
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
public:
	vector<Asm_Cmd*> Cmds; 
	vector<string> *Fmts;
	vector<Asm_Function*> Functions;
	string AddFormat(string new_format);
	void Add(AsmOpType Op, AsmRegType Reg);
	void Add(AsmOpType Op, string Val);
	void Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2);
	void Add(AsmOpType Op, AsmRegType Reg, string Val);
	void Add(AsmOpType Op, string Val, AsmRegType Reg);
	void Add(string Name, string Type, string InitList); 
	void Add(Asm_Function* Func);
	void Add_Addr(AsmOpType Op, string Var, int offset);
	void Add_RAddr(AsmOpType Op, AsmRegType Reg, string Var, int offset);
	void Add_LAddr(AsmOpType Op, string Val, int offset, AsmRegType Reg);
	void Add_RAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2, int offset);
	void Add_LAddr(AsmOpType Op, AsmRegType Reg1, int offset, AsmRegType Reg2);
	void Add_LAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2);
	void Add_RAddr(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2);
	void Print();
	Asm_Code();
};

#endif