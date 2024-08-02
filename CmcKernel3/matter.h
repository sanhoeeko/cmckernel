#pragma once

#include"defs.h"
#include"cardset.h"

struct ConstMatter {
	Formula formula;
	Elems elems;			// accelate computation
	string name;
	string state;
	vector<CardSet>* cardsets;
	Real hsg[3];
	int id;

	ConstMatter();
	ConstMatter(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs);
	Real& g(FreeEnergy mode);
	string toString();
	json dump();
	json dumpWithCardSets(int index);
	ConstMatter* loadWithCardSets(json& js);
	void attachCardSets(json& js);
	vector<__base>& getCardSets();
	bool composeOf(const CardSet& cs);
};

typedef ConstMatter* Matter;

/*
	This class works like std::vector, but allows shallow copy.
	And no memory reallocation.
*/
struct MatterList {
	Matter* data;
	int length;

	MatterList();
	MatterList(Matter* ptr, int n);
	MatterList(const MatterList& o);
	int size();
	void clear();
	void push_back(const Matter m);
	void free();
	Matter& operator[](int idx);
	MatterList sliceBeginAt(int idx);
	MatterList filter(bool(*func)(Matter, Elems), Elems elems);
	MatterList filterCould(Elems elems);
	MatterList filterShould(Elems elems);
	MatterList filterIff(Elems elems);
};

enum class NameType {
	Matter, Cation, Anion
};


struct Cation : public ConstMatter {
	int charge;
	Cation(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs, int charge);
};

struct Anion : public ConstMatter {
	int charge;
	Anion(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs, int charge);
};

ConstMatter* ionicCompound(Cation& c, Anion& a);
string getIonName(string& name);
int getCharge(string& name);

Elems getElements(vector<Matter>& ms);
vector<Matter> filterMatterCouldShould(Elems could_has, Elems should_has, Matter* m, Matter* mend);
// vector<pair<Matter, Matter>> filterMatterPair(Elems elements, Matter* m, Matter* mend);

