#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum AsmOpType { Push = 0, Pop, IMul, Div, Add, Sub, Neg, Not, Or, And, Xor, Shl, Shr, Call, Mov };
enum AsmRegType { EAX = 0, EBX, ECX, EDX, EBP, ESP };

class Asm_Operand {
public:
	virtual string GetCode() { return ""; }
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
	int offset;
	string GetCode();
	Asm_Address(string Val, int offset);
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

class Asm_Code {
private:
	vector<string> Fmts;
	vector<Asm_Cmd*> Cmds;
public:
	string AddFormat(string new_format);
	void Add(AsmOpType Op, AsmRegType Reg);
	void Add(AsmOpType Op, string Str);
	void Add(AsmOpType Op, string Var, int offset);
	void Add(AsmOpType Op, AsmRegType Reg1, AsmRegType Reg2);
	void Add(AsmOpType Op, AsmRegType Reg, string Str);
	void Add(AsmOpType Op, AsmRegType Reg, string Var, int offset);
	void Add(AsmOpType Op, string Str, AsmRegType Reg);
	void Add(AsmOpType Op, string Str, int offset, AsmRegType Reg);
	void Print();
	
};

#endif