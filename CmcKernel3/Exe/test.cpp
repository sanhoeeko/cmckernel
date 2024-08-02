#include <iostream>
using namespace std;

#include "../kernel.h"
#include "../utils.h"

int main() {
    CMCKernel* kernel = new CMCKernel(false);
    //kernel->ReadCMCtable("CMCtable.csv");
    //kernel->GenerateCardSetJson("cardsets.json");
    kernel->ReadCardSetJson("cardsets.json");
    //kernel->GenerateCanReactMatrix("reactions.dat");
    vector<int> input = { 0, 0 };
    vector<int> res = kernel->getPossibleMatters(from_cardIds(input));
    cout << to_string(res) << endl;
}