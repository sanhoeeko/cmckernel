#pragma once

#include"atom.h"
#include<string>

#define ILLEGAL_NAME_CASE 0x0d000721
#define ILLEGAL_ELEMENT_CASE 0x0d000722

Formula parseFormula(const string& _name);

struct Split {
	const char* ptr;
	const char* ptrnxt;
	char* buffer;

	Split(const string& str);
	string next(char delimiter);
	bool goes();
};