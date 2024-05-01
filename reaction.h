#pragma once

#include"matter.h"
#include"numerical.h"
#include"itertools.h"

struct __reaction {
	vector<Matter> reactants;
	vector<Matter> resultants;
	VectorXr coefs;

	__reaction() {
		reactants.resize(2);
		resultants.resize(3);
	}
};

__reaction* reaction_pool = NULL;
__reaction* reaction_ptr;

void clearReactionPool() {
	if (reaction_pool == NULL) {
		reaction_pool = new __reaction[10000];
	}
	reaction_ptr = reaction_pool;
}

struct Reaction {
	__reaction* data;	// to simplify the copy of vector<Reaction>

	Reaction(vector<Matter>& reactants, Matter* resultants, int num_of_resultants, VectorXr& coefs) {
		data = reaction_ptr++;
		data->reactants = reactants;
		memcpy(data->resultants.data(), resultants, num_of_resultants * sizeof(Matter));
		data->resultants.resize(num_of_resultants);
		data->coefs = coefs;
	}
	Real minEnthalpy() {
		Real h = data->resultants[0]->h;
		int n = data->resultants.size();
		for (int i = 1; i < n; i++) {
			if (h > data->resultants[i]->h)h = data->resultants[i]->h;
		}
		return h;
	}
	bool operator<(Reaction& y) {
		return this->minEnthalpy() < y.minEnthalpy();
	}
	string toString() {
		VectorXi vi = integerVec(data->coefs);
		int* p = vi.data();
		string str = to_string(-*p++) + " " + to_string(data->reactants[0]);
		int n1 = data->reactants.size();
		for (int i = 1; i < n1; i++) {
			str += " + " + to_string(-*p++) + " " + to_string(data->reactants[i]);
		}
		str += " -> ";
		str += to_string(*p++) + " " + to_string(data->resultants[0]);
		int n2 = data->resultants.size();
		for (int i = 1; i < n2; i++) {
			str += " + " + to_string(*p++) + " " + to_string(data->resultants[i]);
		}
		return str;
	}
};

string to_string(Reaction& r) {
	return r.toString();
}


struct ReactionSeeker {
	vector<Matter> reactants;
	vector<Matter> possible_matters;
	vector<uc> element_set;
	MatrixXr mat;
	VectorXr hs;
	VectorXr coef_temp;
	bits elements;
	Matter* pm;
	int num_of_reactants, num_of_elements;
	int num_of_resultants = -1;
	int num_of_pm;

	ReactionSeeker(vector<Matter>&& reactants) {
		clearReactionPool();
		this->reactants = reactants;
		this->num_of_reactants = reactants.size();
		this->elements = getElements(reactants);
		this->possible_matters = filterMatterCould(elements, table_start, table_end);
		element_set = bitsToVec(elements);
		num_of_elements = element_set.size();
		pm = possible_matters.data();
		num_of_pm = possible_matters.size();
	}

	void setNumOfResultants(int num_of_resultants) {
		this->num_of_resultants = num_of_resultants;

		// initialize [mat]: each column is a matter
		mat = MatrixXr(num_of_elements, num_of_reactants + num_of_resultants);
		Real* ptr = mat.data();
		for (int i = 0; i < num_of_reactants; i++) {
			reactants[i]->formula.template cast_to_elements<Real>(element_set, ptr + i * num_of_elements);
		}
		// initialize [hs]: enthalpies of reactants and resultants
		hs = VectorXr(num_of_reactants + num_of_resultants);
		for (int i = 0; i < num_of_reactants; i++) {
			hs[i] = reactants[i]->h;
		}
	}

	bool balanceEq(Matter* resultants) {
		/*
			in the solution, reactants have negative coefficients, and resultants have positive coefficients
		*/
		Real* ptr = mat.data() + num_of_reactants * num_of_elements;
		for (int i = 0; i < num_of_resultants; i++) {
			resultants[i]->formula.template cast_to_elements<Real>(element_set, ptr + i * num_of_elements);
		}
		FullPivLU<MatrixXr> solver(mat);
		int dim_of_kernel = solver.dimensionOfKernel();
		if (dim_of_kernel == 1) {
			coef_temp = solver.kernel();
			if (coef_temp[0] > 0)coef_temp *= -1;
			return true;
		}
		else {
			return false;
		}
	}

	void getEnthalpy(Matter* resultants) {
		for (int i = 0; i < num_of_resultants; i++) {
			hs[num_of_reactants + i] = resultants[i]->h;
		}
	}

	bool reactionIsLegal(VectorXr& vec, Matter* resultants) {
		// check if signs of matters are correct
		for (int i = 0; i < num_of_reactants; i++) {
			if (vec[i] > -1e-4)return false;
		}
		for (int i = num_of_reactants; i < num_of_reactants + num_of_resultants; i++) {
			if (vec[i] < 1e-4)return false;
		}
		// calculate enthalpy descent
		getEnthalpy(resultants);
		Real dG = vec.dot(this->hs);
		if (dG > -1e-4)return false;
		return true;
	}

	vector<Reaction> getPossibleReactions1() {
		setNumOfResultants(1);
		vector<Reaction> res;
		vector<Matter> iff_matters = filterMatterIff(elements, pm, pm + num_of_pm);
		int n = iff_matters.size();
		Matter* ifm = iff_matters.data();
		// iter over all possible matters
		for (int i = 0; i < n; i++) {
			if (in(ifm[i], reactants))continue;
			bool can_balance = balanceEq(ifm + i);
			if (!can_balance)continue;
			if (reactionIsLegal(coef_temp, ifm + i)) {
				res.push_back(Reaction(reactants, ifm + i, 1, coef_temp));
			}
		}
		return res;
	}

	vector<Reaction> getPossibleReactions2() {
		setNumOfResultants(2);
		vector<Reaction> res;
		Matter resultant_ptr[2] = {NULL, NULL};
		// iter over all possible matter pairs
		for (int i = 0; i < num_of_pm; i++) {
			Matter a = possible_matters[i];
			if (in(a, reactants))continue;
			resultant_ptr[0] = a;
			bits defect = elements - a->elems;
			vector<Matter> vec_b = filterMatterShould(defect, pm + i + 1, pm + num_of_pm);
			for (Matter b : vec_b) {
				if (in(b, reactants))continue;
				resultant_ptr[1] = b;
				bool can_balance = balanceEq(resultant_ptr);
				if (!can_balance)continue;
				if (reactionIsLegal(coef_temp, resultant_ptr)) {
					res.push_back(Reaction(reactants, resultant_ptr, 2, coef_temp));
				}
			}
		}
		return res;
	}

	vector<Reaction> getPossibleReactions3() {
		setNumOfResultants(3);
		vector<Reaction> res;
		Matter resultant_ptr[3] = { NULL, NULL, NULL };
		for (int i = 0; i < num_of_pm; i++) {
			Matter a = possible_matters[i];
			if (in(a, reactants))continue;
			resultant_ptr[0] = a;
			for (int j = i + 1; j < num_of_pm; j++) {
				Matter b = possible_matters[j];
				if (in(b, reactants))continue;
				resultant_ptr[1] = b;
				bits elems_ab = a->elems | b->elems;
				for (int k = j + 1; k < num_of_pm; k++) {
					Matter c = possible_matters[k];
					if ((elems_ab | c->elems) == elements) {
						if (in(c, reactants))continue;
						resultant_ptr[2] = c;
						bool can_balance = balanceEq(resultant_ptr);
						if (!can_balance)continue;
						if (reactionIsLegal(coef_temp, resultant_ptr)) {
							res.push_back(Reaction(reactants, resultant_ptr, 3, coef_temp));
						}
					}
				}
			}
		}
		return res;
	}

	vector<Reaction> getPossibleReactions() {
		vector<Reaction> v1 = getPossibleReactions1();
		vector<Reaction> v2 = getPossibleReactions2();
		vector<Reaction> v3 = getPossibleReactions3();
		v1 += v2; v1 += v3;
		sort(v1.begin(), v1.end());
		return v1;
	}

	bool canReact() {
		vector<Reaction> v1 = getPossibleReactions1();
		if (!v1.empty())return true;
		vector<Reaction> v2 = getPossibleReactions2();
		if (!v2.empty())return true;
		vector<Reaction> v3 = getPossibleReactions3();
		if (!v3.empty())return true;
		return false;
	}
};

bool canReact(Matter a, Matter b) {
	ReactionSeeker rsker = ReactionSeeker({ a, b });
	return rsker.canReact();
}

vector<Reaction> getPossibleReactions(Matter a, Matter b) {
	ReactionSeeker rsker = ReactionSeeker({ a, b });
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