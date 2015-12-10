#define _CRT_SECURE_NO_WARNINGS

#include "syntaxnodes.h"
#include <vector>
#include <set>
#include "symbol.h"
#include <algorithm>
#include "symtable.h"
#include "checktype.h"
#include <map>

set<TokenType> RelOp = { TK_GREAT, TK_GREAT_EQUAL, TK_LESS, TK_LESS_EQUAL, TK_EQUAL, TK_NOT_EQUAL };

Expr::Expr(TypeExpr TypeExp) : TypeExp(TypeExp){}

ExprBinOp::ExprBinOp(Expr* Left, Token Op, Expr* Right) : Left(Left), Op(Op), Right(Right), Expr(BinExp){}

ExprUnarOp::ExprUnarOp(Token Op, Expr* Exp) : Op(Op), Exp(Exp), Expr(UnarExp){}

ExprConst::ExprConst(Token Value, TypeExpr TypeExp) : Value(Value), Expr(TypeExp){}
ExprBoolConst::ExprBoolConst(Token Value) : ExprConst(Value, ConstBoolExp){}
ExprIntConst::ExprIntConst(Token Value) : ExprConst(Value, ConstIntExp){}
ExprIntConst::ExprIntConst(string Value) : ExprConst(Token(Position(), Value, TK_INTEGER_VALUE), ConstIntExp) {}
ExprDoubleConst::ExprDoubleConst(Token Value) : ExprConst(Value, ConstDoubleExp){}
ExprStringConst::ExprStringConst(Token Value) : ExprConst(Value, ConstStringExp){}

ExprIdent::ExprIdent(Symbol* Sym, Position Pos) : Sym(Sym), Expr(VarExp), Pos(Pos){}

ExprArrayIndex::ExprArrayIndex(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(ArrayExp){}
ExprAssign::ExprAssign(Expr* Left, Expr* Right) : Left(Left), Right(Right), Expr(AssignExp){}
ExprFunction::ExprFunction(Expr* Left, vector<Expr*> Rights) : Left(Left), Rights(Rights), Expr(FunctionExp){}
ExprRecord::ExprRecord(Expr* Left, Symbol* Right) : Left(Left), Right(Right), Expr(RecordExp){}

ExprInitList::ExprInitList(vector<Expr*> List) : Expr(InitExp), List(List) {}

ExprPointer::ExprPointer(Expr* Exp) : Exp(Exp), Expr(PointerExp) {}
ExprDereference::ExprDereference(Expr* Exp) : Exp(Exp), Expr(DereferenceExp) {}

void ExprUnarOp::Print(int Spaces){
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << Op.Source.c_str() << endl;
}

void ExprBinOp::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << Op.Source.c_str() << endl;
	Left->Print(Spaces + 1);
}

void ExprConst::Print(int Spaces){
	print_indent(Spaces);
	cout << Value.Source.c_str() << endl;
}

void ExprIdent::Print(int Spaces){
	print_indent(Spaces);
	cout << Sym->Name << endl;
}

void ExprAssign::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << ":=" << endl;
	Left->Print(Spaces + 1);
}

void ExprFunction::Print(int Spaces){
	for (auto it = Rights.begin(); it < Rights.end(); ++it) {
		(*it)->Print(Spaces + 1);
	}
	print_indent(Spaces);
	cout << "()" << endl;
	Left->Print(Spaces + 1);
}

void ExprRecord::Print(int Spaces){
	print_indent(Spaces + 1);
	cout << Right->Name << endl;
	print_indent(Spaces);
	cout << "." << endl;
	Left->Print(Spaces + 1);
}

void ExprArrayIndex::Print(int Spaces){
	Right->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "[]" << endl;
	Left->Print(Spaces + 1);
}

void ExprInitList::Print(const int Spaces) {
	for (auto it = List.begin(); it < List.end(); ++it) {
		(*it)->Print(Spaces);
	}
}

void ExprPointer::Print(const int Spaces) {
	Exp->Print(Spaces + 1);
	print_indent(Spaces);
	cout << "@" << endl;
} 

void ExprDereference::Print(const int Spaces) {
	print_indent(Spaces);
	cout << "^" << endl;
	Exp->Print(Spaces + 1);
}

void Expr::GetIdentStr(ExpArgList* List){}
	
void ExprBinOp::GetIdentStr(ExpArgList* List){
	Right->GetIdentStr(List);
	Left->GetIdentStr(List);
}

void ExprUnarOp::GetIdentStr(ExpArgList* List){
	Exp->GetIdentStr(List);
}

void ExprConst::GetIdentStr(ExpArgList* List){}

void ExprIdent::GetIdentStr(ExpArgList* List){
	List->Vec.push_back(Sym->Name);
}

void ExprAssign::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void ExprArrayIndex::GetIdentStr(ExpArgList* List){
	Left->GetIdentStr(List);
	Right->GetIdentStr(List);
}

void ExprFunction::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

void ExprRecord::GetIdentStr(ExpArgList* List){
	List->Flag = false;
}

map<TokenType, AsmOpType> BinOperations = {
	make_pair(TK_PLUS, Add), make_pair(TK_MINUS, Sub), make_pair(TK_XOR, Xor), make_pair(TK_OR, Or), make_pair(TK_MUL, IMul), make_pair(TK_AND, And)
};

void ExprBinOp::Generate_Bool_Expr(Asm_Code* Code, ArgState State, AsmOpType Op) {
	string LabelName_1 = Code->GetLocalLabelName();
	string LabelName_2 = Code->GetLocalLabelName();
	Left->Generate(Code, State);
	Code->Add(Pop, EAX);
	Code->Add(Test, EAX, EAX);
	Code->Add(Op, LabelName_1);
	Right->Generate(Code, State);
	Code->Add(Jmp, LabelName_2);
	Code->AddLabel(LabelName_1);
	Code->Add(Push, EAX);
	Code->AddLabel(LabelName_2);
}

#define GetCmd(TypeID, Cmd_f, Cmd_i) (TypeID == TypeID_Double ? Cmd_f : Cmd_i)

void ExprBinOp::Generate_Relation_Expr(Asm_Code* Code, ArgState State) {
	string LabelName_1 = Code->GetLocalLabelName();
	string LabelName_2 = Code->GetLocalLabelName();
	Left->Generate(Code, State);
	if (Left->TypeID != TypeID_Double && Right->TypeID == TypeID_Double) {
		Code->Add_Addr(Fild, dword, ESP);
		Code->Add(Sub, ESP, to_string(4));
		Code->Add_Addr(Fstp, qword, ESP);
	}
	Right->Generate(Code, State);
	if (Left->TypeID == TypeID_Double && Right->TypeID != TypeID_Double) {
		Code->Add_Addr(Fild, dword, ESP);
		Code->Add(Sub, ESP, to_string(4));
		Code->Add_Addr(Fstp, qword, ESP);
	}

	auto TypeID = Left->TypeID == TypeID_Double || Right->TypeID == TypeID_Double ? TypeID_Double : TypeID_Integer;
	if (TypeID == TypeID_Double) {
		Code->Add_Addr(Fld, qword, ESP);
		Code->Add_Addr(Fld, qword, ESP, 8);
		Code->Add(Add, ESP, to_string(16));
	}

	if (Left->TypeID == TypeID_Double || Right->TypeID == TypeID_Double) {
		Code->Add(Fcomip, ST0, ST1);
		Code->Add(Fstp);
	}
	else {
		Code->Add(Pop, EBX);
		Code->Add(Pop, EAX);
		Code->Add(Cmp, EAX, EBX);
	}
	
	switch (Op.Type) {
	case TK_GREAT:
		Code->Add(GetCmd(TypeID, Ja, Jg), LabelName_1);
		break;
	case TK_GREAT_EQUAL:
		Code->Add(GetCmd(TypeID, Jae, Jge), LabelName_1);
		break;
	case TK_LESS:
		Code->Add(GetCmd(TypeID, Jb, Jl), LabelName_1);
		break;
	case TK_LESS_EQUAL:
		Code->Add(GetCmd(TypeID, Jbe, Jle), LabelName_1);
		break;
	case TK_EQUAL:
		Code->Add(Je, LabelName_1);
		break;
	case TK_NOT_EQUAL:
		Code->Add(Jne, LabelName_1);
		break;
	}
	Code->Add(Push, "0");
	Code->Add(Jmp, LabelName_2);
	Code->AddLabel(LabelName_1);
	Code->Add(Push, "1");
	Code->AddLabel(LabelName_2);
}

void ExprBinOp::Generate_Double_Expr(Asm_Code* Code, ArgState State) {
	Right->Generate(Code, State);
	Left->Generate(Code, State);
	if (Left->TypeID == TypeID_Double) {
		Code->Add_Addr(Fld, qword, ESP);
		Code->Add(Add, ESP, to_string(8));
	}
	else {
		Code->Add_Addr(Fild, dword, ESP);
		Code->Add(Add, ESP, to_string(4));
	}

	auto TypeID = Right->TypeID;
	auto Size = TypeID == TypeID_Double ? qword : dword;	
	switch (Op.Type) {
	case TK_PLUS:
		Code->Add_Addr(GetCmd(TypeID, Fadd, Fiadd), Size, ESP);
		break;
	case TK_MINUS:
		Code->Add_Addr(GetCmd(TypeID, Fsub, Fisub), Size, ESP);
		break;
	case TK_MUL:
		Code->Add_Addr(GetCmd(TypeID, Fmul, Fimul), Size, ESP);
		break;
	case TK_DIV:
		Code->Add_Addr(GetCmd(TypeID, Fdiv, Fidiv), Size, ESP);
		break;
	}

	if (Right->TypeID != TypeID_Double) {
		Code->Add(Sub, ESP, to_string(4));
	}
	Code->Add_Addr(Fstp, qword, ESP);
}

void ExprBinOp::Generate(Asm_Code* Code, ArgState State) {
	if (RelOp.find(Op.Type) != RelOp.end()) {
		Generate_Relation_Expr(Code, State);
		return;
	}
	if (TypeID == TypeID_Double) {
		Generate_Double_Expr(Code, State);
		return;
	}
	if (Left->TypeID == TypeID_Boolean && (Op.Type == TK_AND || Op.Type == TK_OR)) {
		if (Op.Type == TK_AND) {
			Generate_Bool_Expr(Code, State, Jz);
		}
		if (Op.Type == TK_OR) {
			Generate_Bool_Expr(Code, State, Jnz);
		}
		return;
	}

	Left->Generate(Code);
	if (Op.Type == TK_SHL || Op.Type == TK_SHR) { /* shl/shr eax, A */
		Code->Add(Pop, EAX);				      /* A - const only */
		if (Right->TypeExp != ConstIntExp) {
			throw ConstIntExpressionExpected(Op.Pos);
		}
		Code->Add(Op.Type == TK_SHL ? Shl : Shr, EAX, ((ExprIntConst*)Right)->Value.Source);
		Code->Add(Push, EAX);
		return;
	}
	Right->Generate(Code);
	Code->Add(Pop, EBX);
	Code->Add(Pop, EAX);
	if (Op.Type == TK_DIV_INT || Op.Type == TK_MOD) {
		Code->Add(Xor, EDX, EDX);
		Code->Add(Div, EBX);
		Op.Type == TK_DIV_INT ? Code->Add(Push, EAX) : Code->Add(Push, EDX);
		return;
	}
	for (auto it = BinOperations.cbegin(); it != BinOperations.cend(); ++it) {
		if ((*it).first == Op.Type) {
			Code->Add((*it).second, EAX, EBX);
			Code->Add(Push, EAX);
			return;
		}
	}
}

void ExprUnarOp::Generate(Asm_Code* Code, ArgState State) {
	switch (Op.Type) {
	case TK_PLUS: 
		return;
	case TK_MINUS:	
		Exp->Generate(Code);
		Code->Add(Pop, EAX);
		Code->Add(Neg, EAX);
		Code->Add(Push, EAX);
		return;
	case TK_NOT: 
		Exp->Generate(Code);
		Code->Add(Pop, EAX);
		Code->Add(Not, EAX);
		Code->Add(Push, EAX);
		return;
	}
}

void ExprBoolConst::Generate(Asm_Code* Code, ArgState State) {
	Code->Add(Push, _stricmp(Value.Source.c_str(), "true") == 0 ? "1" : "0");
}

void ExprIntConst::Generate(Asm_Code* Code, ArgState State) {
	Code->Add(Push, Value.Source);
}

void ExprIntConst::ConvertToDouble(Asm_Code* Code, ArgState State) {
	Code->Add_Addr(Fild, dword, ESP);
	Code->Add(Sub, ESP, to_string(4));
	Code->Add_Addr(Fstp, qword, ESP);
}

void ExprDoubleConst::Generate(Asm_Code* Code, ArgState State) {
	string Name = Code->AddDoubleVar(Value.Source);
	Code->Add_RAddr(Mov, EAX, Name, 4);
	Code->Add(Push, EAX);
	Code->Add_RAddr(Mov, EAX, Name, 0);
	Code->Add(Push, EAX);
}

void ExprStringConst::Generate(Asm_Code* Code, ArgState State) {
	int Size = Value.Source.size();
	if (Size == 1) {
		Code->Add(Push, '\'' + Value.Source + '\'');
		return;
	}
	for (int i = 0; i < Size; i += 4) {
		Code->Add(Mov, EAX, '\'' + Value.Source.substr(i, min(4, Size - i)) + '\'');
		Code->Add_LAddr(Mov, "base_str", i, EAX);
	}
	Code->Add(Mov, EAX, "0x0");
	Code->Add_LAddr(Mov, "base_str", Size, EAX);
	Code->Add(Push, "base_str");
}

static void PushRValue(Asm_Code* Code, int Size) {
	for (int i = Size - 4; i >= 0; i -= 4) {
		Code->Add_RAddr(Mov, EBX, EAX, i);
		Code->Add(Push, EBX);
	}
}

void ExprIdent::Generate(Asm_Code* Code, ArgState State) {
	auto IdenSym = (SymIdent*)Sym;
	if (IdenSym->isLocal) {
		if (IdenSym->depth < Code->depth) {
			Code->Add_RAddr(Mov, EAX, "depth", 4 * IdenSym->depth);
		}
		else {
			Code->Add(Mov, EAX, EBP);
		}
		Code->Add(Add, EAX, to_string(IdenSym->offset));
	}
	else {
		Code->Add(Mov, EAX, IdenSym->GenerateName());
	}
	if ((IdenSym->State == Var || IdenSym->State == Const)) {
		Code->Add_RAddr(Mov, EAX, EAX);
	}
	if (State == RValue) {
		PushRValue(Code, IdenSym->GetSize());
		return;
	}
	Code->Add(Push, EAX);
}

void ExprAssign::Generate(Asm_Code* Code, ArgState State) {
	Right->Generate(Code);
	Left->Generate(Code, Var);
	Code->Add(Pop, EAX);
	if (Left->TypeID == TypeID_Double && Right->TypeID != TypeID_Double) {
		Right->ConvertToDouble(Code);
	}
	int Size = max(Left->GetSize(), Right->GetSize());
	for (int i = 0; i < Size; i += 4) {
		Code->Add(Pop, EBX);
		Code->Add_LAddr(Mov, EAX, i, EBX);
	}
}

void ExprArrayIndex::Generate(Asm_Code* Code, ArgState State) {
	Left->Generate(Code, Var);
	Right->Generate(Code);
	Code->Add(Pop, EBX);
	int Size = GetSize();
	int Low = GetBound(0).first;
	Code->Add(Sub, EBX, to_string(Low));
	Code->Add(IMul, EBX, to_string(Size));
	if (Left->TypeExp == FunctionExp) {
		Code->Add(Mov, EAX, ESP);
		Code->Add(Add, EAX, EBX);
		Code->Add(Add, ESP, to_string(Left->GetSize()));
	}
	else {
		Code->Add(Pop, EAX);
		Code->Add(Add, EAX, EBX);
	}
	if (State == RValue) {
		PushRValue(Code, Size);
		return;
	}
	Code->Add(Push, EAX);
}

void ExprRecord::Generate(Asm_Code* Code, ArgState State) {
	Left->Generate(Code, Var);
	int offset = ((SymIdent*)Right)->offset;
	if (Left->TypeExp == FunctionExp) {
		Code->Add(Mov, EAX, ESP);
		Code->Add(Add, EAX, to_string(offset));
		Code->Add(Add, ESP, to_string(Left->GetSize()));
	}
	else {
		Code->Add(Pop, EAX);
		Code->Add(Add, EAX, to_string(offset));
	}
	if (State == RValue) {
		PushRValue(Code, Right->GetSize());
		return;
	}
	Code->Add(Push, EAX);
}

void ExprFunction::Generate(Asm_Code* Code, ArgState State) {
	SymCall* LSym = (SymCall*)((ExprIdent*)Left)->Sym;
	int argc = LSym->argc;
	if (argc >= 0) {
		if (LSym->Section == DeclFunction) {
			Code->Add(Sub, ESP, to_string((LSym->Table->Symbols[argc - 1])->GetSize()));	
		}
		Code->Add(Mov, EAX, "depth");
		Code->Add_LAddr(Mov, EAX, 4 * Code->depth, EBP);
		for (int i = 0; i < Rights.size(); ++i) {
			Rights[i]->Generate(Code, ((SymIdent*)(LSym->Table->Symbols[i]))->State);
		}
		Code->Add(Call, LSym->GenerateName());
	}

	if (argc == argc_write || argc == argc_writeln){
		vector<MyTypeID> TypeIDexp;
		for (int i = Rights.size() - 1; i >= 0; --i) {
			Rights[i]->Generate(Code);
			TypeIDexp.push_back(CheckType(((SymProcedure*)((ExprIdent*)Left)->Sym)->Table, Position()).GetTypeID(Rights[i]));
		}
		string format = "\'";
		for (int i = 0; i < Rights.size(); ++i) {
			switch (TypeIDexp[TypeIDexp.size() - 1 - i]) {
			case TypeID_Integer:
			case TypeID_Pointer:
				format += "%d";
				break;
			case TypeID_Char:
				format += "%c";
				break;
			case TypeID_String:
				format += "%s";
				break;
			case TypeID_Double:
				format += "%f";
				break;
			}
		}
		format += argc == argc_writeln ? "\', 0xA, 0x0" : "\', 0x0";
		string FormatName = Code->AddFormat(format);

		Code->Add(Push, FormatName); 
		Code->Add(Call, "_printf");
		int Size = 0;
		for (int i = 0; i < Rights.size(); ++i) {
			Size += Rights[i]->GetSize();
		}
		Code->Add(Add, ESP, to_string(Size + 4));
	}
}

void ExprInitList::Generate(Asm_Code* Code, ArgState State) {
	for (int i = List.size() - 1; i >= 0; --i) {
		List[i]->TypeID = TypeID;
		List[i]->Generate(Code);
	}
}

void ExprPointer::Generate(Asm_Code* Code, ArgState State) {
	Exp->Generate(Code, RValue);
}

void ExprDereference::Generate(Asm_Code* Code, ArgState State) {
	Exp->Generate(Code, RValue);
}

string ExprConst::GenerateInitList() {
	return Value.Source;
}

string ExprBoolConst::GenerateInitList() {
	return _stricmp(Value.Source.c_str(), "true") == 0 ? "1" : "0";
}

string ExprInitList::GenerateInitList() {
	string Ans = List[0]->GenerateInitList(); 
	for (int i = 1; i < List.size(); ++i) {
		Ans += ", " + List[i]->GenerateInitList();
	}
	return Ans;
}

int ExprBinOp::GetSize() {
	return max(Left->GetSize(), Right->GetSize());
}

int ExprUnarOp::GetSize() {
	return Exp->GetSize();
}

int ExprConst::GetSize() {
	return 4;
}

int ExprDoubleConst::GetSize() {
	return 8;
}

int ExprIdent::GetSize() {
	return Sym->GetSize();
}

pair<int, int> ExprIdent::GetBound(int depth) {
	Symbol* SymId = Sym->GetType();
	for (int i = 0; i < depth - 1; ++i) {
		SymId = SymId->GetType();
	}
	return make_pair(((SymDynArray*)SymId)->GetLow(), ((SymDynArray*)SymId)->GetHigh());
}

int ExprArrayIndex::GetSize() {
	auto Bound = Left->GetBound(1);
	int Size = Left->GetSize();
	return Left->GetSize() / (Bound.second - Bound.first + 1);
}

pair<int, int> ExprArrayIndex::GetBound(int depth) {
	return Left->GetBound(depth + 1);
}

int ExprFunction::GetSize() {
	return Left->GetSize();
}

pair<int, int> ExprFunction::GetBound(int depth) {
	return Left->GetBound(depth);
}

int ExprRecord::GetSize() {
	return Right->GetSize();
}

pair<int, int> ExprRecord::GetBound(int depth) {
	Symbol* SymId = Right->GetType();
	for (int i = 0; i < depth - 1; ++i) {
		SymId = SymId->GetType();
	}
	return make_pair(((SymDynArray*)SymId)->GetLow(), ((SymDynArray*)SymId)->GetHigh());
}

int ExprInitList::GetSize() {
	return List[0]->GetSize();
}

int ExprDereference::GetSize() {
	return Exp->GetSize();
}

pair<int, int> ExprDereference::GetBound(int depth) {
	return Exp->GetBound(depth);
}