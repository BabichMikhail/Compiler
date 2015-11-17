#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "parser.h"

class Asm_Code {
public:
	virtual string GetCode() { return ""; };
};

class Asm_Cmd : public Asm_Code{
public:
	AsmOp Cmd;
	Asm_Cmd(AsmOp Cmd);
};

class Asm_Bin_Cmd : public Asm_Cmd {
public:
	Asm_Code* Oper1;
	Asm_Code* Oper2;
	string GetCode();
	Asm_Bin_Cmd(AsmOp Cmd, Asm_Code* Oper1, Asm_Code* Oper2);
};

class Asm_Unar_Cmd : public Asm_Cmd {
public:
	Asm_Code* Oper1;
	string GetCode();
	Asm_Unar_Cmd(AsmOp Cmd, Asm_Code* Oper1);
};

class Asm_Operand : public Asm_Code {};

class Asm_Registr : public Asm_Operand{
public:
	AsmRegistr Reg;
	string GetCode();
	Asm_Registr(AsmRegistr Reg);
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

class Asm_Generator {
private:
	vector<Asm_Code*> Code;
public:
	Parser* CodeParser;
	void SetCode();
	void CodeToStdout();
	Asm_Generator(const char* filename, PMod State);
};

#endif