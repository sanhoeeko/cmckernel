#pragma once

#include"parse.h"
#include<fstream>

class ConstMatter {
public:
	Formula formula;
	string name;
	string state;
	float h;			// enthalpy

	ConstMatter(string& name, string& state, string& enthalpy) {
		this->formula = formulaFromName(name);
		this->name = name;
		this->state = state;
		this->h = stof(enthalpy);
	}
};

typedef const ConstMatter* Matter;

const ConstMatter* table;

inline int getMatterId(Matter m) {
	return m - table;
}

void ReadCMCtable(const char* filename) {
	ifstream fin(filename);
	ConstMatter* current_matter;
	bool first = true;
	if (!fin.is_open()) {
		cout << "CMCTable file does not exist!" << endl;
		throw "File not exists";
	}
	string line;
	std::getline(fin, line);		// obmit the first line
	while (std::getline(fin, line)) {
		Split sp = Split(line);
		string name = sp.next(',');
		string state = sp.next(',');
		string enthalpy = sp.next(',');
		try {
			current_matter = new ConstMatter(name, state, enthalpy);
			if (first) {
				table = current_matter;
				first = false;
			}
			current_matter++;
		}
		catch (int exception) {
			if (exception == ILLEGAL_NAME_CASE || exception ==  ILLEGAL_ELEMENT_CASE) {
				;
			}
			else {
				throw "Real exception in parsing.";
			}
		}
	}
}