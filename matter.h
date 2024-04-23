#pragma once

#include"parse.h"
#include<fstream>
#include<algorithm>

class ConstMatter {
public:
	Formula formula;
	bits elems;			// accelate computation
	string name;
	string state;
	float h;			// enthalpy

	ConstMatter(string& name, string& state, string& enthalpy) {
		this->formula = formulaFromName(name);
		this->elems = getElements(formula);
		this->name = name;
		this->state = state;
		this->h = stof(enthalpy);
	}
};

typedef ConstMatter* Matter;

vector<ConstMatter*> table;

inline int getMatterId(Matter m) {
	return m - table[0];
}

inline string to_string(Matter m) {
	return m->name + m->state;
}

inline bits getElements(vector<Matter>& ms) {
	bits res = 0;
	for (auto m : ms) {
		res |= m->elems;	// get union of elements
	}
	return res;
}

void ReadCMCtable(const char* filename) {
	ifstream fin(filename);
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
			table.push_back(new ConstMatter(name, state, enthalpy));
		}
		catch (int exception) {
			if (exception == ILLEGAL_NAME_CASE || exception ==  ILLEGAL_ELEMENT_CASE) {
				;	// do not take this matter into account
			}
			else {
				throw "Real exception in parsing.";
			}
		}
	}
}

vector<Matter> filterMatterSufficient(bits elements) {
	/*
		if the set of elements is a sufficient condition of the matter
	*/
	vector<Matter> res;
	for (Matter m : table) {
		if ((m->elems | elements) == elements) {
			res.push_back(m);
		}
	}
	return res;
}

vector<Matter> filterMatterIff(bits elements) {
	/*
		if the set of elements is an equivalent condition of the matter
	*/
	vector<Matter> res;
	for (Matter m : table) {
		if (m->elems == elements) {
			res.push_back(m);
		}
	}
	return res;
}