#pragma once

#include"parse.h"
#include"defs.h"
#include<fstream>
#include<algorithm>

class ConstMatter {
public:
	Formula formula;
	bits elems;			// accelate computation
	string name;
	string state;
	int id;
	Real h;			// enthalpy

	ConstMatter(int id, string& name, string& state, string& enthalpy) {
		this->formula = formulaFromName(name);
		this->elems = getElements(formula);
		this->name = name;
		this->state = state;
		this->id = id;
		this->h = stof(enthalpy);
	}
};

typedef ConstMatter* Matter;

vector<ConstMatter*> table;
ConstMatter** table_start;
ConstMatter** table_end;


inline string to_string(Matter m) {
	// return m->name + m->state;
	return m->name;
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
	if (!fin.is_open()) {
		cout << "CMCTable file does not exist!" << endl;
		throw "File not exists";
	}
	string line;
	std::getline(fin, line);		// omit the first line
	while (std::getline(fin, line)) {
		Split sp = Split(line);
		string name = sp.next(',');
		string state = sp.next(',');
		string enthalpy = sp.next(',');
		try {
			bool already_has = false;
			for (auto it = table.begin(); it != table.end(); ++it) {
				if ((*it)->name == name) {
					if ((*it)->h <= stof(enthalpy)) {
						already_has = true;
					}
					else {
						table.erase(it);
					}
					break;
				}
			}
			if (!already_has) {
				table.push_back(new ConstMatter(0, name, state, enthalpy));
			}
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
	table_start = table.data();
	table_end = table.data() + table.size();
	for (int i = 0; i < table.size(); i++) {
		table[i]->id = i;
	}
	fin.close();
}

Matter getMatterByName(const string& name) {
	for (Matter* m = table_start; m < table_end; m++) {
		if ((*m)->name == name)return *m;
	}
	return NULL;
}

vector<Matter> filterMatterCould(bits elements, Matter* m, Matter* mend) {
	/*
		if the elements of the matter is a subset of the set of elements
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if (((*m)->elems | elements) == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterShould(bits elements, Matter* m, Matter* mend) {
	/*
		if the elements of the matter is a subset of the set of elements
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if (((*m)->elems & elements) == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterIff(bits elements, Matter* m, Matter* mend) {
	/*
		if the set of elements is an equivalent condition of the matter
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if ((*m)->elems == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterCouldShould(bits could_has, bits should_has, Matter* m, Matter* mend) {
	/*
		the matter could has a set of elements, and must has another set of elements
	*/
	vector<Matter> res;
	for (; m < table_end; m++) {
		if (((*m)->elems | could_has) == could_has && ((*m)->elems & should_has) == should_has) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<pair<Matter, Matter>> filterMatterPair(bits elements) {
	/*
		return all possible pairs of matters (A, B), where elems(A) | elems(B) == elements
		The intuitive method is O(N^2), where N is the size of CMC table,
		however, this method is slightly faster.
	*/
	vector<pair<Matter, Matter>> res;
	vector<Matter> vec_a = filterMatterCould(elements, table_start, table_end);
	int n = vec_a.size();
	Matter* pstart = vec_a.data();
	Matter* pend = pstart + n;
	for (int i = 0; i < n; i++) {
		Matter a = vec_a[i];
		bits defect = elements - a->elems;
		vector<Matter> vec_ab = filterMatterShould(defect, pstart + i + 1, pend);
		for (auto b : vec_ab) {
			if (b->formula != a->formula) {
				res.push_back(make_pair(a, b));
			}
		}
	}
	return res;
}

vector<Matter> getAllPossibleMatters(vector<uc> cards) {
	static vector<uc> element_cards;
	static vector<uc> number_cards;
	static vector<uc> other_cards;
	splitCardsByType(cards, element_cards, number_cards, other_cards);
	vector<Formula> fs = getAllPossibleFormulas(element_cards, number_cards);
	bits elements = getElements(element_cards);
	vector<Matter> sub_table = filterMatterIff(elements, table_start, table_end);
	vector<Matter> res;
	for (Matter m : sub_table) {
		for (Formula& f : fs) {
			if (f == m->formula) {
				res.push_back(m); break;
			}
		}
	}
	return res;
}