#include "Optimizer.h"

set<AsmOpType> BinCmd = { Add, Sub, Xor, Or, IMul, And };
set<AsmOpType> UnarCmd = { IMul, Div };

void CutCmd(vector<Asm_Cmd*>* Cmds, vector<Asm_Cmd*>::iterator it, int Count) {
	for (auto it_2 = it; it_2 < Cmds->end() - Count; ++it_2) {
		*it_2 = *(it_2 + Count);
	}
	for (int i = 0; i < Count; ++i) {
		Cmds->pop_back();
	}
}

bool Cmp_CmdIndex(vector<Asm_Cmd*>::iterator it, int it_offset, AsmCmdIndex Idx) {
	return (*(it + it_offset))->CmdIndex == Idx;
}

// Old:
//     pop eax
//     CMD eax, ebx
//     push eax
// New:
//     CMD [esp + 4], ebx
bool Template_0(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 2; ++it) {
		if (Cmp_CmdIndex(it, 0, Cmd_Register) && Cmp_CmdIndex(it, 1, Cmd_Register_Register) && Cmp_CmdIndex(it, 2, Cmd_Register) &&
			(*it)->Op == Pop && (*(it + 2))->Op == Push) {
			
			auto Reg_1 = (Asm_Register*)((Asm_Unar_Cmd*)*it)->Oper1;
			auto Reg_2 = (Asm_Register*)((Asm_Unar_Cmd*)*(it + 2))->Oper1;
			auto Reg_3 = (Asm_Register*)((Asm_Bin_Cmd*)*(it + 1))->Oper1;
			if (Reg_1 == Reg_2 && Reg_1 == Reg_3) {
				((Asm_Bin_Cmd*)*(it + 1))->Oper1 = new Asm_Address(ESP, 4);
				*it = *it + 1;
				CutCmd(Cmds, it + 1, 2);
				return true;
			}
		}
	}
	return false;
}

// Old:
//     pop Reg
//     push Reg
bool Template_1(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Pop && (*(it + 1))->Op == Push) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register) && Cmp_CmdIndex(it, 1, Cmd_Register)) {
				auto Reg_1 = ((Asm_Register*)((Asm_Unar_Cmd*)*it)->Oper1)->Reg;
				auto Reg_2 = ((Asm_Register*)((Asm_Unar_Cmd*)*(it + 1))->Oper1)->Reg;
				if (Reg_1 == Reg_2) {
					CutCmd(Cmds, it, 2);
					return true;
				}
			}
		}
	}
	return false;
}

// Old:
//     push Reg_1
//     pop Reg_2
bool Template_2(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Push && (*(it + 1))->Op == Pop) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register) && Cmp_CmdIndex(it, 1, Cmd_Register)) {
				auto Reg_1 = ((Asm_Register*)((Asm_Unar_Cmd*)*it)->Oper1)->Reg;
				auto Reg_2 = ((Asm_Register*)((Asm_Unar_Cmd*)*(it + 1))->Oper1)->Reg;
				if (Reg_1 == Reg_2) { // Reg_1 == Reg_2
					CutCmd(Cmds, it, 2);
					return true;
				}
				else { // Reg_1 != Reg_2
					*it = new Asm_Bin_Cmd(Mov, new Asm_Register(Reg_2), new Asm_Register(Reg_1), Cmd_Register_Register);
					CutCmd(Cmds, it + 1, 1);
					return true;
				}
			}
		}
	}
	return false;
}

// Old:
//    mov Reg, A
//    push Size [Reg]
// New: 
//    push Size [A]
bool Template_3(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Mov && (*(it + 1))->Op == Push) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register_Ident) && Cmp_CmdIndex(it, 1, Cmd_Size_Addr_Register_Offset)) {
				auto Cmd_1 = (Asm_Bin_Cmd*)*it;
				auto Cmd_2 = (Asm_Address*)((Asm_Unar_Size_Cmd*)*(it + 1))->Oper1;
				if (((Asm_Register*)Cmd_1->Oper1)->Reg == ((Asm_Register*)Cmd_2->Oper)->Reg && Cmd_2->offset == 0) { 
					Cmd_2->Oper = new Asm_Variable(*(Asm_Variable*)Cmd_1->Oper2);
					CutCmd(Cmds, it, 1);
					return true;
				}
			}
		}
	}
	return false;
}

// Old:
//    mov Reg, A
//    push Size [Reg + 4]
//    push Size [Reg]
// New: 
//    push Size [A + 4]
//    push Size [A]
bool Template_4(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Mov && (*(it + 1))->Op == Push) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register_Ident) && Cmp_CmdIndex(it, 1, Cmd_Size_Addr_Register_Offset) && Cmp_CmdIndex(it, 2, Cmd_Size_Addr_Register_Offset)) {
				auto Cmd_1 = (Asm_Bin_Cmd*)*it;
				auto Cmd_2 = (Asm_Address*)((Asm_Unar_Size_Cmd*)*(it + 1))->Oper1;
				auto Cmd_3 = (Asm_Address*)((Asm_Unar_Size_Cmd*)*(it + 2))->Oper1;
				if (((Asm_Register*)Cmd_1->Oper1)->Reg == ((Asm_Register*)Cmd_2->Oper)->Reg && Cmd_2->offset == 4 && Cmd_3->offset == 0) {
					Cmd_2->Oper = new Asm_Variable(*(Asm_Variable*)Cmd_1->Oper2);
					Cmd_3->Oper = new Asm_Variable(*(Asm_Variable*)Cmd_1->Oper2);
					CutCmd(Cmds, it, 1);
					return true;
				}
			}
		}
	}
	return false;
}

// Old:
//     push Int_1
//     push Int_2
//     pop Reg_2
//     pop Reg_1
// New:
//     mov Reg_1, Int_1
//     mov Reg_2, Int_2
bool Template_5(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 3; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Push && (*(it + 1))->Op == Push && (*(it + 2))->Op == Pop && (*(it + 3))->Op == Pop) {
			if (Cmp_CmdIndex(it, 0, Cmd_Int) && Cmp_CmdIndex(it, 1, Cmd_Int) && Cmp_CmdIndex(it, 2, Cmd_Register) && Cmp_CmdIndex(it, 3, Cmd_Register)) {
				auto Int_1 = ((Asm_IntConst*)((Asm_Unar_Cmd*)*it)->Oper1)->Val;
				auto Int_2 = ((Asm_IntConst*)((Asm_Unar_Cmd*)*(it + 1))->Oper1)->Val;
				auto Reg_2 = ((Asm_Register*)((Asm_Unar_Cmd*)*(it + 2))->Oper1)->Reg;
				auto Reg_1 = ((Asm_Register*)((Asm_Unar_Cmd*)*(it + 3))->Oper1)->Reg;
				*it = new Asm_Bin_Cmd(Mov, new Asm_Register(Reg_1), new Asm_IntConst(Int_1), Cmd_Register_Int);
				 *(it + 1) = new Asm_Bin_Cmd(Mov, new Asm_Register(Reg_2), new Asm_IntConst(Int_2), Cmd_Register_Int);
				CutCmd(Cmds, it + 2, 2);
				return true;
			}
		}
	}
	return false;
}

// Old:
//     push Int
//     pop Reg
// New:
//     mov Reg, Int
bool Template_6(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Push && (*(it + 1))->Op == Pop) {
			if (Cmp_CmdIndex(it, 0, Cmd_Int) && Cmp_CmdIndex(it, 1, Cmd_Register)) {
				auto Int = ((Asm_IntConst*)((Asm_Unar_Cmd*)*it)->Oper1)->Val;
				auto Reg = ((Asm_Register*)((Asm_Unar_Cmd*)*(it + 1))->Oper1)->Reg;
				*it = new Asm_Bin_Cmd(Mov, new Asm_Register(Reg), new Asm_IntConst(Int), Cmd_Register_Int);
				CutCmd(Cmds, it + 1, 1);
				return true;
			}
		}
	}
	return false;
}

// Old:
//     mov Reg_1, Int_1
//     cmd Reg_2, Reg_1
// New:
//     cmd Reg_2, Int_1
bool Template_7(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Mov && BinCmd.find((*(it + 1))->Op) != BinCmd.end()) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register_Int) && Cmp_CmdIndex(it, 1, Cmd_Register_Register)) {
				auto Reg_1 = ((Asm_Register*)((Asm_Bin_Cmd*)*it)->Oper1)->Reg;
				auto Reg_2 = ((Asm_Register*)((Asm_Bin_Cmd*)*(it + 1))->Oper2)->Reg;
				if (Reg_1 != Reg_2) {
					continue;
				}
				auto Oper = ((Asm_Bin_Cmd*)*it)->Oper2;
				((Asm_Bin_Cmd*)*(it + 1))->Oper2 = Oper;
				(*(it + 1))->CmdIndex = Cmd_Register_Int;
				CutCmd(Cmds, it, 1);
				return true;
			}
		}
	}
	return false;
}

// Old:
//     add Reg, 0 || sub Reg, 0
bool Template_8(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end(); ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if (((*it)->Op == Add || (*it)->Op == Sub) && Cmp_CmdIndex(it, 0, Cmd_Register_Int) && ((Asm_IntConst*)((Asm_Bin_Cmd*)*it)->Oper2)->Val == "0") {
			CutCmd(Cmds, it, 1);
			return true;
		}
	}
	return false;
}

// Old:
//     imul Reg, 1 || div Reg, 1
bool Template_9(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end(); ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == IMul && Cmp_CmdIndex(it, 0, Cmd_Register_Int) && ((Asm_IntConst*)((Asm_Bin_Cmd*)*it)->Oper2)->Val == "1") {
			CutCmd(Cmds, it, 1);
			return true;
		}
	}
	return false;
}

// Old:
//     pop Reg_1
//     cmd Reg_1, Reg_3 ; cmd != imul
//     push Reg_1
// New: 
//     mov [esp], Reg_3
bool Template_10(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 2; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if ((*it)->Op == Pop && (*(it + 2))->Op == Push && BinCmd.find((*(it + 1))->Op) != BinCmd.end() && (*(it + 1))->Op != IMul) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register) && Cmp_CmdIndex(it, 1, Cmd_Register_Register) && Cmp_CmdIndex(it, 2, Cmd_Register)) {
				auto Reg_1 = ((Asm_Register*)((Asm_Unar_Cmd*)(*it))->Oper1)->Reg;
				auto Reg_2 = ((Asm_Register*)((Asm_Bin_Cmd*)(*(it + 1)))->Oper1)->Reg;
				auto Reg_3 = ((Asm_Register*)((Asm_Bin_Cmd*)(*(it + 1)))->Oper2)->Reg;
				auto Reg_4 = ((Asm_Register*)((Asm_Unar_Cmd*)(*(it + 2)))->Oper1)->Reg;
				if (Reg_1 == Reg_2 && Reg_1 == Reg_4 && Reg_1 != Reg_3) {
					*it = new Asm_Bin_Cmd((*(it + 1))->Op, new Asm_Address(ESP, 0), new Asm_Register(Reg_3), Cmd_Addr_Register_Register);
					CutCmd(Cmds, it + 1, 2);
					return true;
				}
			}
		}
	}
	return false;
}

// Old:
//     cmd Reg_2, Int
//     cmd [Reg_1], Reg_2
// New: 
//     cmd dword [Reg_1], Int
bool Template_11(vector<Asm_Cmd*>* Cmds) {
	for (auto it = Cmds->begin(); it < Cmds->end() - 1; ++it) {
		if ((*it)->CmdIndex == Label) {
			continue;
		}
		if (BinCmd.find((*it)->Op) != BinCmd.end() && (*(it + 1))->Op == Mov) {
			if (Cmp_CmdIndex(it, 0, Cmd_Register_Int) && Cmp_CmdIndex(it, 1, Cmd_Addr_Register_Register)) {
				auto Reg_1 = ((Asm_Register*)((Asm_Bin_Cmd*)(*it))->Oper1)->Reg;
				auto Reg_3 = ((Asm_Register*)((Asm_Bin_Cmd*)(*(it + 1)))->Oper2)->Reg;
				if (Reg_1 == Reg_3) {
					*it = new Asm_Bin_Left_Size_Cmd((*(it + 1))->Op, dword, ((Asm_Bin_Cmd*)(*(it + 1)))->Oper1, ((Asm_Bin_Cmd*)(*it))->Oper2, Cmd_Size_Addr_Register_Register);
					CutCmd(Cmds, it + 1, 1);
					return true;
				}
			}
		}
	}
	return false;
}

Optimizer::Optimizer(Asm_Code* Code) : Code(Code) {
	Templates.push_back(&Template_0);
	Templates.push_back(&Template_1); 
	Templates.push_back(&Template_2);
	Templates.push_back(&Template_3);
	Templates.push_back(&Template_4);
	Templates.push_back(&Template_5);
	Templates.push_back(&Template_6);
	Templates.push_back(&Template_7);
	Templates.push_back(&Template_8);
	Templates.push_back(&Template_9);
	Templates.push_back(&Template_10);
	Templates.push_back(&Template_11);
}

void Optimizer::Optimize_Func(Asm_Function* Func) {
	if (Func->Cmds.size()) {
		Optimize_Code(&Func->Cmds);
	}
	for each (auto F in Func->Functions) {
		Optimize_Func(F);
	}
}

void Optimizer::Optimize_Code(vector<Asm_Cmd*>* Cmds) {
	bool flag;
	do {
		flag = false;
		for each (auto Func in Templates) {
			flag |= Func(Cmds);
		}
	} while (flag);
}

void Optimizer::Optimize() {
	if (Code->Cmds.size()) {
		Optimize_Code(&Code->Cmds);
	}
	for each (auto Func in Code->Functions) {
		Optimize_Func(Func);
	}
}
