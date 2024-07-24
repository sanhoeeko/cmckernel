#include "matter.h"
#include "cardset.h"

string getState(string& state) {
	char c = state[1];
	switch (c)
	{
	case 's':return "(s)";
	case 'l':return "(l)";
	case 'g':return "(g)";
	case 'a':return "(aq)";
	default:return "(?)";
	}
}

ConstMatter::ConstMatter(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs)
{
	this->formula = parseFormula(name);
	this->elems = formula.getElements();
	this->name = name;
	this->state = getState(state);
	this->id = id;
	hsg[0] = enthalpy; hsg[1] = -entropy /* negative sign */; hsg[2] = gibbs;
}

Real& ConstMatter::g(FreeEnergy mode)
{
	return hsg[mode];
}

string ConstMatter::toString()
{
	return name + state;
}

json ConstMatter::dumpWithCardSets()
{
	json js;
	vector<CardSet> sets = possibleCardSets(formula);
	js["id"] = id;
	js["name"] = toString();
	for (auto& s : sets) {
		js["cards"].push_back(s.toJson());
	}
	return js;
}

inline Elems getElements(vector<Matter>& ms) {
	Elems res = 0;
	for (auto m : ms) {
		res |= m->elems;	// get union of elements
	}
	return res;
}

vector<Matter> filterMatterCould(Elems elements, Matter* m, Matter* mend) {
	/*
		if the elements of the matter is a subset of the set of elements
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if (((*m)->elems | elements) == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterShould(Elems elements, Matter* m, Matter* mend) {
	/*
		if the elements of the matter is a subset of the set of elements
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if (((*m)->elems & elements) == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterIff(Elems elements, Matter* m, Matter* mend) {
	/*
		if the set of elements is an equivalent condition of the matter
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if ((*m)->elems == elements) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<Matter> filterMatterCouldShould(Elems could_has, Elems should_has, Matter* m, Matter* mend) {
	/*
		the matter could has a set of elements, and must has another set of elements
	*/
	vector<Matter> res;
	for (; m < mend; m++) {
		if (((*m)->elems | could_has) == could_has && ((*m)->elems & should_has) == should_has) {
			res.push_back((*m));
		}
	}
	return res;
}

vector<pair<Matter, Matter>> filterMatterPair(Elems elements, Matter* m, Matter* mend) {
	/*
		return all possible pairs of matters (A, B), where elems(A) | elems(B) == elements
		The intuitive method is O(N^2), where N is the size of CMC table,
		however, this method is slightly faster.
	*/
	vector<pair<Matter, Matter>> res;
	vector<Matter> vec_a = filterMatterCould(elements, m, mend);
	int n = vec_a.size();
	Matter* pstart = vec_a.data();
	Matter* pend = pstart + n;
	for (int i = 0; i < n; i++) {
		Matter a = vec_a[i];
		Elems defect = elements - a->elems;
		vector<Matter> vec_ab = filterMatterShould(defect, pstart + i + 1, pend);
		for (auto b : vec_ab) {
			if (b->formula != a->formula) {
				res.push_back(make_pair(a, b));
			}
		}
	}
	return res;
}

vector<Matter> getAllPossibleMatters(vector<uc> cards, Matter* m, Matter* mend) {
	static vector<uc> element_cards;
	static vector<uc> number_cards;
	static vector<uc> other_cards;
	splitCardsByType(cards, element_cards, number_cards, other_cards);
	vector<Formula> fs = getAllPossibleFormulas(element_cards, number_cards);
	Elems elements = getElements(element_cards);
	vector<Matter> sub_table = filterMatterIff(elements, m, mend);
	vector<Matter> res;
	for (Matter m : sub_table) {
		for (Formula& f : fs) {
			if (f == m->formula) {
				res.push_back(m); break;
			}
		}
	}
	return res;
}