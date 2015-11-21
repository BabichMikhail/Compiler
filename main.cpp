#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "errors.h"
#include "AsmGenerator.h"

using namespace std;

#define print_format \
	cout << "Usage: Compiler [-h] [-help] [-p] [-l] [-d] [-s] [-g]" << endl << endl; \
	cout << "The most commontly used input and output filenames" << endl << endl


int main(int argc, char **argv){
	if (argc == 1){
		print_format;
		return 0;
	}

	if (argc == 2){
		if (strcmp(argv[1], "-help") == 0 || (strcmp(argv[1], "-h") == 0)){
			print_format;
		}
		return 0;
	}

	if (argc == 3){
		print_format;
		return 0;
	}

	if (argc > 4){
		cout << "Too many arguments" << endl << endl;
		print_format;
	}
	ofstream output;
	output.open(argv[3]);
	output.clear();
	cout.rdbuf(output.rdbuf());
	cerr.rdbuf(output.rdbuf());
	try {
		if (argc == 4){
			if (strcmp(argv[1], "-l") == 0){
				Lexer LXR(argv[2]);
				while (LXR.isToken()) {
					LXR.Next();
					LXR.Print();
				}
			}

			if (strcmp(argv[1], "-p") == 0){
				Parser P(argv[2], Test_Exp);
				P.Print();
			}

			if (strcmp(argv[1], "-d") == 0){
				Parser P(argv[2], Test_Decl);
				P.Print();
			}

			if (strcmp(argv[1], "-s") == 0) {
				Parser P(argv[2], Test_Statement);
				P.Print();
			}

			if (strcmp(argv[1], "-g") == 0) {
				Parser P(argv[2], Test_Statement);
				Asm_Code *Code = new Asm_Code();
				P.Stmt->GetAsmCode(Code);
				Code->Print();
			}
		}
	}
	catch (Error e){
		cerr << e.err_msg << endl;
	}
	return 0;
}