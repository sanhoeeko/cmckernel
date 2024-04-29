#include "strategy.h"

#include<iostream>


int main() {
	ReadCMCtable("CMCtable.csv");
	vector<uc> cards = { 1, 1, 7, 7, 8, 64, 64, 65 };
	CardHolder ch;
	ch.updateHand(cards);
	StrategyList st = ch.getStrategies(getMatterByName("O2"));
	cout << to_string(st) << endl;
}