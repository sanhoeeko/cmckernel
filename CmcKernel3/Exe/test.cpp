#include <iostream>
using namespace std;

#include "../cardset.h"
#include "../parse.h"

int main() {
    Formula f = parseFormula("(NH4)2SO4");
    cout << f.toString() << endl;
    vector<CardSet> cs = possibleCardSets(f);
    cout << "***" << endl;
    for (auto& c : cs) {
        cout << c.toString() << endl;
    }
}