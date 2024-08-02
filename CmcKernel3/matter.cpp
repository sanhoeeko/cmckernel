#include "pch.h"
#include "parse.h"
#include "matter.h"

inline static string getState(string& state) {
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

ConstMatter::ConstMatter()
{
}

ConstMatter::ConstMatter(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs)
{
	this->formula = parseFormula(name);
	this->elems = formula.getElements();
	this->name = name;
	this->state = getState(state);
	this->id = id;
	hsg[0] = enthalpy; hsg[1] = -entropy /* negative sign */; hsg[2] = gibbs;
	cardsets = NULL;
}

Real& ConstMatter::g(FreeEnergy mode)
{
	return hsg[mode];
}

string ConstMatter::toString()
{
	return name + state;
}

json ConstMatter::dump()
{
	json js;
	js["id"] = id;
	js["name"] = name;
	js["state"] = state;
	js["h"] = hsg[0];
	js["_s"] = hsg[1];
	js["g"] = hsg[2];
	return js;
}

json ConstMatter::dumpWithCardSets(int index)
{
	json js = dump();
	js["id"] = index;
	vector<CardSet> sets = possibleCardSets(formula);
	for (auto& s : sets) {
		js["cards"].push_back(s.toJson());
	}
	return js;
}

ConstMatter* ConstMatter::loadWithCardSets(json& js)
{
	this->formula = parseFormula(js["name"]);
	this->elems = formula.getElements();
	this->name = js["name"];
	this->state = js["state"];
	this->id = js["id"];
	hsg[0] = js["h"]; hsg[1] = js["_s"] /* negative sign */; hsg[2] = js["g"];
	attachCardSets(js["cards"]);
	return this;
}

void ConstMatter::attachCardSets(json& js)
{
	int n = js.size();
	cardsets = new vector<__base>();
	cardsets->reserve(n);
	for (int i = 0; i < n; i++) {
		cardsets->push_back(from_vector(js[i].get<vector<vector<uc>>>()));
	}
}

vector<__base>& ConstMatter::getCardSets()
{
	return *cardsets;
}

bool ConstMatter::composeOf(const CardSet& cs)
{
	for (auto& o : *cardsets) {
		if (o == cs)return true;
	}
	return false;
}

Elems getElements(vector<Matter>& ms) {
	Elems res = 0;
	for (auto m : ms) {
		res |= m->elems;	// get union of elements
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

//vector<pair<Matter, Matter>> filterMatterPair(Elems elements, Matter* m, Matter* mend) {
//	/*
//		return all possible pairs of matters (A, B), where elems(A) | elems(B) == elements
//		The intuitive method is O(N^2), where N is the size of CMC table,
//		however, this method is slightly faster.
//	*/
//	vector<pair<Matter, Matter>> res;
//	vector<Matter> vec_a = filterMatterCould(elements, m, mend);
//	int n = vec_a.size();
//	Matter* pstart = vec_a.data();
//	Matter* pend = pstart + n;
//	for (int i = 0; i < n; i++) {
//		Matter a = vec_a[i];
//		Elems defect = elements - a->elems;
//		vector<Matter> vec_ab = filterMatterShould(defect, pstart + i + 1, pend);
//		for (auto b : vec_ab) {
//			if (b->formula != a->formula) {
//				res.push_back(make_pair(a, b));
//			}
//		}
//	}
//	return res;
//}

MatterList::MatterList()
{
	data = new Matter[65536];	// never free
	length = 0;
}

MatterList::MatterList(Matter* ptr, int n)
{
	data = ptr;
	length = n;
}

MatterList::MatterList(const MatterList& o)
{
	data = o.data;				// shallow copy
	length = o.length;
}

int MatterList::size()
{
	return length;
}

void MatterList::clear()
{
	length = 0;
}

void MatterList::push_back(const Matter m)
{
	data[length++] = m;
}

void MatterList::free()
{
	delete[] data;
}

Matter& MatterList::operator[](int idx)
{
	return data[idx];
}

MatterList MatterList::sliceBeginAt(int idx)
{
	return { data + idx, length - idx };
}

/*
	`filter` will create a new `MatterList`, including memory allocation.
*/
MatterList MatterList::filter(bool(*func)(Matter, Elems), Elems elems)
{
	MatterList output;
	for (int i = 0; i < length; i++) {
		if (func(data[i], elems))output.push_back(data[i]);
	}
	return output;
}

MatterList MatterList::filterCould(Elems elems)
{
	return filter([](Matter m, Elems e) {return (m->elems | e) == e; }, elems);
}

MatterList MatterList::filterShould(Elems elems)
{
	return filter([](Matter m, Elems e) {return (m->elems & e) == e; }, elems);
}

MatterList MatterList::filterIff(Elems elems)
{
	return filter([](Matter m, Elems e) {return m->elems == e; }, elems);
}
