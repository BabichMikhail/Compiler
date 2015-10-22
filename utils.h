#ifndef UTILS_H
#define UTILS_H

#include <vector>

using namespace std;

typedef struct ExpArgList{
	vector<string> Vec;
	bool Flag;
	ExpArgList() : Flag(true){};
} ExpArgList;

#endif