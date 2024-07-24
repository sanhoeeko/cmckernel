#pragma once

#include"defs.h"
#include"parse.h"
#include<fstream>
#include<algorithm>

class ConstMatter {
public:
	Formula formula;
	Elems elems;			// accelate computation
	string name;
	string state;
	Real hsg[3];
	int id;

	ConstMatter(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs);
	Real& g(FreeEnergy mode);
	string toString();
	json dumpWithCardSets();
};

typedef ConstMatter* Matter;
