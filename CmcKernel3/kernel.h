#pragma once

#include "strategy.h"

struct CMCKernel {
	CardHolder* hand;
	Advisor* advisor;
	MatterList table;
	vector<Cation*> cations;
	vector<Anion*> anions;

	CMCKernel(bool full);
	
	// IO
	void ReadCMCtable(const char* filename);
	void GenerateCanReactMatrix(const char* out_file_name);
	void GenerateCardSetJson(const char* out_file_name);
	void ReadCardSetJson(const char* filename);

	// callables
	vector<int> getPossibleMatters(const CardSet& cs);

	// auxiliary
	void addMatterToTable(ConstMatter* m);
	bool canReact(Matter a, Matter b);

	vector<Reaction> getPossibleReactions(Matter a, Matter b);
	vector<Matter> calculateMattersCanReactWith(Matter a);
};
