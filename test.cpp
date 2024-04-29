#include "strategy.h"

#include<iostream>


int main() {
	ReadCMCtable("CMCtable.csv");
	// GenerateCanReactMatrix("can_react.matrix");
	Matter a = getMatterByName("FeS");
	/*
	Matter b = getMatterByName("CH3COOH");
	auto v = getPossibleReactions(a, b);
	for (auto& r : v) {
		cout << to_string(r) << endl;
	}*/
	checkCanReactMatrix(a);
}