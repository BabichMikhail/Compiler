#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "errors.h"

using namespace std;

int main(int argc, char **argv){
	if (argc == 1){
		cout << "Missing Arguments" << endl;
		cout << "Unknown argument. Write [-h] or [-help] for get help" << endl;
		return 0;
	}

	if (argc == 2){
		if (strcmp(argv[1], "-help") == 0 || (strcmp(argv[1], "-h") == 0)){
			cout << "Format arguments:" << endl;
			cout << "  1. Key: -l (lexer), -p (parser)" << endl;
			cout << "  2. Input" << endl;
			cout << "  3. Output" << endl;
		}
		return 0;
	}

	if (argc == 3){
		cout << "Missing Output File" << endl;
		return 0;
	}

	if (argc > 4){
		cout << "Too many arguments" << endl;
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
				Parser P(argv[2]);
				P.Print();
			}
		}
	}
	catch (Error e){
		cerr << e.err_msg << endl;
	}
	return 0;
}