#pragma once

#include"matter.h"
#include <numeric>

enum class NameType {
	Matter, Cation, Anion
};

class Cation : public ConstMatter {
public:
	int charge;
	Cation(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs, int charge)
		: ConstMatter(id, name, state, enthalpy, entropy, gibbs) {
		this->charge = charge;
	}
};

class Anion : public ConstMatter {
public:
	int charge;
	Anion(int id, const string& name, string& state, Real enthalpy, Real entropy, Real gibbs, int charge)
		: ConstMatter(id, name, state, enthalpy, entropy, gibbs) {
		this->charge = charge;
	}
};


bool isOnlyOneUpperCase(const char* ptr) {
	int cnt = 0;
	while (*ptr) {
		if (*ptr >= 'A' && *ptr <= 'Z') {
			if (cnt == 1)return false;
			cnt++;
		}
		ptr++;
	}
	if (cnt == 0)return false;
	return true;
}

string nameMul(string& name, int mul) {
	if (mul == 1)return name;
	if (isOnlyOneUpperCase(name.c_str()))return name + to_string(mul);
	return "(" + name + ")" + to_string(mul);
}

ConstMatter* ionicCompound(Cation& c, Anion& a) {
	static string state = "(aq)";
	assert(c.state == state && a.state == state);
	int LCM = std::lcm(c.charge, a.charge);
	int c_coef = LCM / c.charge;
	int a_coef = LCM / a.charge;
	string ionic_name = nameMul(c.name, c_coef) + nameMul(a.name, a_coef);
	Real h = c_coef * c.g(H) + a_coef * a.g(H);
	Real s = -(c_coef * c.g(_S) + a_coef * a.g(_S));
	Real g = c_coef * c.g(G) + a_coef * a.g(G);
	return new ConstMatter(0, ionic_name, state, h, s, g);
}

int getCharge(string& name) {
	const char* ptr = name.c_str();
	while (*ptr) {
		if (*ptr == '+') {
			return stoi(string(1, ptr[1]));
		}
		else if (*ptr == '-') {
			return -stoi(string(1, ptr[1]));
		}
		ptr++;
	}
	return 0;
}

string getIonName(string& name) {
	const char* ptr = name.c_str();
	char* buffer = new char[name.size()];
	char* bufptr = buffer;
	while (*ptr) {
		if (*ptr == '+' || *ptr == '-') {
			break;
		}
		*bufptr++ = *ptr++;
	}
	*bufptr = '\0';
	string res= string(buffer);
	delete[] buffer;
	return res;
}