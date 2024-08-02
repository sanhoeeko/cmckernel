#include "pch.h"
#include "reaction.h"
#include "utils.h"

__reaction* reaction_pool = NULL;
__reaction* reaction_ptr;

__reaction::__reaction()
{
	reactants.resize(2);
	resultants.resize(3);
}

void clearReactionPool() {
	if (reaction_pool == NULL) {
		reaction_pool = new __reaction[10000];
	}
	reaction_ptr = reaction_pool;
}

Reaction::Reaction(vector<Matter>& reactants, Matter* resultants, int num_of_resultants, VectorXr& coefs)
{
	data = reaction_ptr++;
	data->reactants = reactants;
	memcpy(data->resultants.data(), resultants, num_of_resultants * sizeof(Matter));
	data->resultants.resize(num_of_resultants);
	data->coefs = coefs;
}

Real Reaction::minEnthalpy()
{
	Real h = data->resultants[0]->g(G);
	int n = data->resultants.size();
	for (int i = 1; i < n; i++) {
		if (h > data->resultants[i]->g(G))h = data->resultants[i]->g(G);
	}
	return h;
}

bool Reaction::operator<(Reaction& y)
{
	return this->minEnthalpy() < y.minEnthalpy();
}

string Reaction::toString()
{
	VectorXi vi = integerVec(data->coefs);
	int* p = vi.data();
	string str = to_string(-*p++) + " " + data->reactants[0]->toString();
	int n1 = data->reactants.size();
	for (int i = 1; i < n1; i++) {
		str += " + " + to_string(-*p++) + " " + data->reactants[i]->toString();
	}
	str += " -> ";
	str += to_string(*p++) + " " + data->resultants[0]->toString();
	int n2 = data->resultants.size();
	for (int i = 1; i < n2; i++) {
		str += " + " + to_string(*p++) + " " + data->resultants[i]->toString();
	}
	return str;
}

ReactionSeeker::ReactionSeeker(vector<Matter>&& reactants, MatterList& table)
{
	clearReactionPool();
	this->reactants = reactants;
	this->num_of_reactants = reactants.size();
	this->elements = getElements(reactants);
	this->table = table;
	this->possible_matters = table.filterCould(elements);
	element_set = to_vector(elements);
	num_of_elements = element_set.size();
}

void ReactionSeeker::setNumOfResultants(int num_of_resultants)
{
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
		hs[i] = reactants[i]->g(G);
	}
}

bool ReactionSeeker::balanceEq(Matter* resultants)
{
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

void ReactionSeeker::getEnthalpy(Matter* resultants)
{
	for (int i = 0; i < num_of_resultants; i++) {
		hs[num_of_reactants + i] = resultants[i]->g(G);
	}
}

bool ReactionSeeker::reactionIsLegal(VectorXr& vec, Matter* resultants)
{
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

vector<Reaction> ReactionSeeker::getPossibleReactions1()
{
	setNumOfResultants(1);
	vector<Reaction> res;
	MatterList iff_matters = possible_matters.filterIff(elements);
	// iter over all possible matters
	for (int i = 0; i < iff_matters.size(); i++) {
		if (in(iff_matters[i], reactants))continue;
		bool can_balance = balanceEq(&iff_matters[i]);
		if (!can_balance)continue;
		if (reactionIsLegal(coef_temp, &iff_matters[i])) {
			res.push_back(Reaction(reactants, &iff_matters[i], 1, coef_temp));
		}
	}
	iff_matters.free();
	return res;
}

vector<Reaction> ReactionSeeker::getPossibleReactions2()
{
	setNumOfResultants(2);
	vector<Reaction> res;
	Matter resultant_ptr[2] = { NULL, NULL };
	// iter over all possible matter pairs
	for (int i = 0; i < possible_matters.size(); i++) {
		Matter a = possible_matters[i];
		if (in(a, reactants))continue;
		resultant_ptr[0] = a;
		Elems defect = elements - a->elems;
		MatterList vec_b = possible_matters.sliceBeginAt(i + 1).filterShould(defect);
		for (int j = 0; j < vec_b.size(); j++) {
			Matter b = vec_b[j];
			if (in(b, reactants))continue;
			resultant_ptr[1] = b;
			bool can_balance = balanceEq(resultant_ptr);
			if (!can_balance)continue;
			if (reactionIsLegal(coef_temp, resultant_ptr)) {
				res.push_back(Reaction(reactants, resultant_ptr, 2, coef_temp));
			}
		}
		vec_b.free();
	}
	return res;
}

vector<Reaction> ReactionSeeker::getPossibleReactions3()
{
	setNumOfResultants(3);
	vector<Reaction> res;
	int n = possible_matters.size();
	Matter resultant_ptr[3] = { NULL, NULL, NULL };
	for (int i = 0; i < n; i++) {
		Matter a = possible_matters[i];
		if (in(a, reactants))continue;
		resultant_ptr[0] = a;
		for (int j = i + 1; j < n; j++) {
			Matter b = possible_matters[j];
			if (in(b, reactants))continue;
			resultant_ptr[1] = b;
			Elems elems_ab = a->elems | b->elems;
			for (int k = j + 1; k < n; k++) {
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

vector<Reaction> ReactionSeeker::getPossibleReactions()
{
	vector<Reaction> v1 = getPossibleReactions1();
	vector<Reaction> v2 = getPossibleReactions2();
	vector<Reaction> v3 = getPossibleReactions3();
	v1 += v2; v1 += v3;
	sort(v1.begin(), v1.end());
	return v1;
}

bool ReactionSeeker::canReact1()
{
	setNumOfResultants(1);
	MatterList iff_matters = possible_matters.filterIff(elements);
	// iter over all possible matters
	for (int i = 0; i < iff_matters.size(); i++) {
		if (in(iff_matters[i], reactants))continue;
		bool can_balance = balanceEq(&iff_matters[i]);
		if (!can_balance)continue;
		if (reactionIsLegal(coef_temp, &iff_matters[i])) {
			return true;
		}
	}
	iff_matters.free();
	return false;
}

bool ReactionSeeker::canReact2()
{
	setNumOfResultants(2);
	Matter resultant_ptr[2] = { NULL, NULL };
	// iter over all possible matter pairs
	for (int i = 0; i < possible_matters.size(); i++) {
		Matter a = possible_matters[i];
		if (in(a, reactants))continue;
		resultant_ptr[0] = a;
		Elems defect = elements - a->elems;
		MatterList vec_b = possible_matters.sliceBeginAt(i + 1).filterShould(defect);
		for (int j = 0; j < vec_b.size(); j++) {
			Matter b = vec_b[j];
			if (in(b, reactants))continue;
			resultant_ptr[1] = b;
			bool can_balance = balanceEq(resultant_ptr);
			if (!can_balance)continue;
			if (reactionIsLegal(coef_temp, resultant_ptr)) {
				return true;
			}
		}
		vec_b.free();
	}
	return false;
}

bool ReactionSeeker::canReact3()
{
	setNumOfResultants(3);
	int n = possible_matters.size();
	Matter resultant_ptr[3] = { NULL, NULL, NULL };
	for (int i = 0; i < n; i++) {
		Matter a = possible_matters[i];
		if (in(a, reactants))continue;
		resultant_ptr[0] = a;
		for (int j = i + 1; j < n; j++) {
			Matter b = possible_matters[j];
			if (in(b, reactants))continue;
			resultant_ptr[1] = b;
			Elems elems_ab = a->elems | b->elems;
			for (int k = j + 1; k < n; k++) {
				Matter c = possible_matters[k];
				if ((elems_ab | c->elems) == elements) {
					if (in(c, reactants))continue;
					resultant_ptr[2] = c;
					bool can_balance = balanceEq(resultant_ptr);
					if (!can_balance)continue;
					if (reactionIsLegal(coef_temp, resultant_ptr)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool ReactionSeeker::canReact()
{
	return canReact1() || canReact2() || canReact3();
}