#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "asmgenerator.h"

typedef bool (*TemplFunc)(vector<Asm_Cmd*>* Cmds);

class Optimizer {
private:
	Asm_Code* Code;
	vector<TemplFunc> Templates;
	void Optimize_Func(Asm_Function* Func);
	void Optimize_Code(vector<Asm_Cmd*>* Cmds);
public:
	Optimizer(Asm_Code* Code);
	void Optimize();
};

#endif