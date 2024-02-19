#include"matter.h"

#include<iostream>

int main() {
	ReadCMCtable("CMCtable.csv");

	/*
	vector<int> cards = { 1, 8, 64, 65 };
	vector<Formula> res = geoAllPossibleFormulas(cards);
	for (auto& f : res) {
		cout << to_string(f) << endl;
	}
	return 0;
	*/
	
	string name = "(NH4)2SiO4";
	Formula f = formulaFromName(name);
	cout << to_string(f) << endl;
}