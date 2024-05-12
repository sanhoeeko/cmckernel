#pragma once

#include"strategy.h"

struct CMCKernel {
	CardHolder* hand;
	Advisor* advisor;
	vector<ConstMatter*> table;
	ConstMatter** table_start;
	ConstMatter** table_end;
	vector<Cation*> cations;
	vector<Anion*> anions;

	CMCKernel() {
		ReadCMCtable("CMCtable.csv");
		advisor = new Advisor(table);
		hand = new CardHolder(advisor);
	}

	Matter getMatterByName(const string& name) {
		for (Matter* m = table_start; m < table_end; m++) {
			if ((*m)->name == name)return *m;
		}
		return NULL;
	}
	void addMatterToTable(ConstMatter* m) {
		bool already_has = false;
		for (auto it = table.begin(); it != table.end(); ++it) {
			if ((*it)->name == m->name) {
				if ((*it)->g(G) <= m->g(G)) {
					already_has = true;
				}
				else {
					table.erase(it);
				}
				break;
			}
		}
		if (!already_has) {
			table.push_back(m);
		}
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
		table_start = table.data();
		table_end = table.data() + table.size();
		for (int i = 0; i < table.size(); i++) {
			table[i]->id = i;
		}
		fin.close();
	}

	bool canReact(Matter a, Matter b) {
		ReactionSeeker rsker = ReactionSeeker({ a, b }, table);
		return rsker.canReact();
	}

	vector<Reaction> getPossibleReactions(Matter a, Matter b) {
		ReactionSeeker rsker = ReactionSeeker({ a, b }, table);
		return rsker.getPossibleReactions();
	}
	vector<Matter> calculateMattersCanReactWith(Matter a) {
		vector<Matter> ms;
		for (Matter m : table) {
			if (canReact(m, a)) {
				ms.push_back(m);
			}
		}
		return ms;
	}

	void GenerateCanReactMatrix(const char* out_file_name) {
		int n = table.size();
		int num_of_pairs = ((n - 1) * (n - 2)) / 2;
		vector<char> arr; arr.reserve(num_of_pairs);
		Matter* ptr = table_start;
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (canReact(table[i], table[j])) {
					arr.push_back(1);
				}
				else {
					arr.push_back(0);
				}
			}
			cout << "Calculated line " << i << endl;
		}
		writeCanReactMatrix(arr, out_file_name);
	}
};
CMCKernel* kernel;
