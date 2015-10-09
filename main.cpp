#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv){
	if (argc == 1)
		printf("Missing Arguments\n");

	if (argc == 2){
		if (strcmp(argv[1], "-help") == 0 || (strcmp(argv[1], "-h") == 0)){

		}
		else {
			printf("Unknown argument. Write [-h] or [-help] for get help\n");
		}
	}
	if (argc == 3){
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
		};
	}
	if (argc == 4){
		if (strcmp(argv[1], "-tl") == 0){
			std::ofstream output;
			output.open(argv[3]);
			output.clear();
			std::cout.rdbuf(output.rdbuf());
			Lexer LXR(argv[2]);
			while (LXR.isToken()) {
				LXR.Next();
				LXR.Print();
			}
			output.close();
		}
		if (strcmp(argv[1], "-tp") == 0){
			std::ofstream output;
			output.open(argv[3]);
			output.clear();
			std::cout.rdbuf(output.rdbuf());
			Parser P(argv[2]);
			P.Print();
			output.close();
		}
	}
	return 0;
}