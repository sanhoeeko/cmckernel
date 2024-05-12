#include "interface.h"

#include<iostream>
using namespace std;


int main() {
	Init();
	cout << kernel->advisor->reactAbilityOf(kernel->getMatterByName("CuS")) << endl;
}