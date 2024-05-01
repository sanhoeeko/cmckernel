#include "interface.h"

#include<iostream>
using namespace std;


int main() {
	ReadCMCtable("CMCtable.csv");
	cout << QueryPossibleMatters("C H H 2") << endl;
}