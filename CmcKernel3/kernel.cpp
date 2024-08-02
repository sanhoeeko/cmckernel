#include "pch.h"
#include "kernel.h"
#include "parse.h"
#include <fstream>
#include "utils.h"


CMCKernel::CMCKernel(bool full) {
	{
		if (full) {
			advisor = new Advisor(table);
			hand = new CardHolder(advisor);
			// check the integrity of data
			assert(table.size() == advisor->num_of_matter);
		}
	}
}

void CMCKernel::addMatterToTable(ConstMatter* m)
{
	bool already_has = false;
	for (int i = 0; i < table.size(); i++) {
		if (table[i]->name == m->name) {
			if (table[i]->g(G) <= m->g(G)) {
				// already has a more stable one
				return;
			}
			else {
				table[i] = m;  // replace
				return;
			}
		}
	}
	table.push_back(m);		// add
}

void CMCKernel::ReadCMCtable(const char* filename)
{
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
		string entropy = sp.next(',');
		string gibbs = sp.next(',');
		try {
			int charge = getCharge(name);
			if (charge == 0) {
				addMatterToTable(new ConstMatter(0, name, state, stod(enthalpy), stod(entropy), stod(gibbs)));
			}
			else {
				if (state == "(aq)") {
					if (charge > 0) {
						cations.push_back(new Cation(0, getIonName(name), state, stod(enthalpy), stod(entropy), stod(gibbs), charge));
					}
					else {
						anions.push_back(new Anion(0, getIonName(name), state, stod(enthalpy), stod(entropy), stod(gibbs), -charge));
					}
				}
			}
		}
		catch (int exception) {
			if (exception == ILLEGAL_NAME_CASE || exception == ILLEGAL_ELEMENT_CASE) {
				;	// do not take this matter into account
			}
			else {
				throw "Real exception in parsing.";
			}
		}
	}
	// generate matters from cations and anions
	for (Cation*& c : cations) {
		for (Anion*& a : anions) {
			addMatterToTable(ionicCompound(*c, *a));
		}
	}
	// read Ksp table and substitute insoluble salts

	// add id
	for (int i = 0; i < table.size(); i++) {
		table[i]->id = i;
	}
	fin.close();
	cout << "Successfully load " << table.size() << " matters" << endl;
}

bool CMCKernel::canReact(Matter a, Matter b)
{
	ReactionSeeker rsker = ReactionSeeker({ a, b }, table);
	return rsker.canReact();
}

vector<Reaction> CMCKernel::getPossibleReactions(Matter a, Matter b)
{
	ReactionSeeker rsker = ReactionSeeker({ a, b }, table);
	return rsker.getPossibleReactions();
}

vector<Matter> CMCKernel::calculateMattersCanReactWith(Matter a)
{
	{
		vector<Matter> ms;
		for (int i = 0; i < table.size(); i++) {
			if (canReact(table[i], a)) {
				ms.push_back(table[i]);
			}
		}
		return ms;
	}
}

void CMCKernel::GenerateCanReactMatrix(const char* out_file_name)
{
	int n = table.size();
	int num_of_pairs = ((n - 1) * (n - 2)) / 2;
	vector<char> arr; arr.resize(num_of_pairs);
	std::fill(arr.begin(), arr.end(), 64);
	char* ptr = arr.data();
#pragma omp parallel for num_threads(4)
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			int idx = i * (2 * n - i - 1) / 2 + (j - i - 1);
			if (ptr[idx] == 64) {
				ptr[idx] = canReact(table[i], table[j]);
			}
			else {
				cout << "Error in generating reactions.matrix." << endl;
				// throw "Error in generating reactions.matrix.";
			}
		}
		cout << "Calculated line " << i << endl;
	}
	// check data integrity
	if (in((char)64, arr)) {
		cout << "Error in generating reactions.matrix." << endl;
		throw "Error in generating reactions.matrix.";
	}
	writeCanReactMatrix(arr, out_file_name);
}

void CMCKernel::GenerateCardSetJson(const char* out_file_name)
{
	json js;
	int cnt = 0;
	for (int i = 0; i < table.size(); i++) {
		// matters counted larger than 20 or 28 are espicially difficult to calculate
		auto matter = table[i];
		if (matter->formula.count() <= 20) {
			cout << cnt << ": " << matter->name << endl;
			js.push_back(matter->dumpWithCardSets(cnt));
			cnt++;
		}
	}
	writeJson(js, out_file_name);
}

/*
	while `ReadCMCTable reads ions, this function reads caculated matters.
*/
void CMCKernel::ReadCardSetJson(const char* filename)
{
	table.clear();
	json js = readJson(filename);
	for (auto& jItem : js) {
		table.push_back((new ConstMatter())->loadWithCardSets(jItem));
	}
	cout << "Successfully load " << table.size() << " matters" << endl;
}

/*
	return: matter IDs
*/
vector<int> CMCKernel::getPossibleMatters(const CardSet& cs)
{
	vector<int> res;
	Elems elems = cs.getElements();
	MatterList ml = table.filterIff(elems);
	for (int i = 0; i < ml.size();i++) {
		if (ml[i]->composeOf(cs))res.push_back(ml[i]->id);
	}
	return res;
}
