#include "matter.h"

#include<iostream>

int main() {
	/*
	vector<uc> cards = { 1, 8, 64, 65 };
	vector<Formula> res = geoAllPossibleFormulas(cards);
	for (auto& f : res) {
		cout << to_string(f) << endl;
	}
	return 0;
	*/
	/*
	string name = "(NH4)2SiO4";
	Formula f = formulaFromName(name);
	cout << to_string(f) << endl;
	*/
	ReadCMCtable("CMCtable.csv");
	auto vec = filterMatterIff(((bits)1 << 1) + ((bits)1 << 8));
	for (auto m : vec) {
		cout << to_string(m) << endl;
	}
}