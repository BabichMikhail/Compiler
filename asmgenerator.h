#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum AsmOp { AsmPush = 0, AsmPop, AsmIMul, AsmDiv, AsmAdd, AsmSub, AsmNeg, AsmNot, AsmOr, AsmAnd, AsmXor, AsmShl, AsmShr, AsmCall, AsmMov };
enum AsmRegistr { AsmEAX = 0, AsmEBX, AsmECX, AsmEDX, AsmEBP, AsmESP };

class Asm_Operand {
public:
	virtual string GetCode() { return ""; }
};

class Asm_Registr : public Asm_Operand{
public:
	AsmRegistr Reg;
	string GetCode();
	Asm_Registr(AsmRegistr Reg);
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
	AsmOp Op;
	virtual string GetCode() = 0;
	Asm_Cmd(AsmOp Cmd);
};

class Asm_Bin_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	Asm_Operand* Oper2;
	string GetCode();
	Asm_Bin_Cmd(AsmOp Op, Asm_Operand* Oper1, Asm_Operand* Oper2);
};

class Asm_Unar_Cmd : public Asm_Cmd {
public:
	Asm_Operand* Oper1;
	string GetCode();
	Asm_Unar_Cmd(AsmOp Op, Asm_Operand* Oper1);
};

class Asm_Code {
private:
	vector<Asm_Cmd*> Cmds;
public:
	void Add(AsmOp Op, AsmRegistr Reg);
	void Add(AsmOp Op, string Str);
	void Add(AsmOp Op, string Var, int offset);
	void Add(AsmOp Op, AsmRegistr Reg1, AsmRegistr Reg2);
	void Add(AsmOp Op, AsmRegistr Reg, string Str);
	void Add(AsmOp Op, AsmRegistr Reg, string Var, int offset);
	void Add(AsmOp Op, string Str, AsmRegistr Reg);
	void Add(AsmOp Op, string Str, int offset, AsmRegistr Reg);
	void Print();
	
};

#endif