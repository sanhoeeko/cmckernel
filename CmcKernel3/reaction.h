#pragma once

#include"matter.h"

struct __reaction {
	vector<Matter> reactants;
	vector<Matter> resultants;
	VectorXr coefs;
	__reaction();
};

struct Reaction {
	__reaction* data;	// to simplify the copy of vector<Reaction>

	Reaction(vector<Matter>& reactants, Matter* resultants, int num_of_resultants, VectorXr& coefs);
	Real minEnthalpy();
	bool operator<(Reaction& y);
	string toString();
};


struct ReactionSeeker {
	vector<Matter> reactants;
	vector<uc> element_set;
	MatrixXr mat;
	VectorXr hs;
	VectorXr coef_temp;
	Elems elements;
	MatterList table;
	MatterList possible_matters;
	int num_of_reactants, num_of_elements;
	int num_of_resultants = -1;

	ReactionSeeker(vector<Matter>&& reactants, MatterList& table);
	void setNumOfResultants(int num_of_resultants);
	bool balanceEq(Matter* resultants);
	void getEnthalpy(Matter* resultants);
	bool reactionIsLegal(VectorXr& vec, Matter* resultants);
	vector<Reaction> getPossibleReactions1();
	vector<Reaction> getPossibleReactions2();
	vector<Reaction> getPossibleReactions3();
	vector<Reaction> getPossibleReactions();
	bool canReact1();
	bool canReact2();
	bool canReact3();
	bool canReact();
};