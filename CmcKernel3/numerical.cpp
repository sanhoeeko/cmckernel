#include "pch.h"
#include"utils.h"
#include<numeric>
using namespace std;

const int fraction_depth = 8;	// the deeper, the more accurate
const Real nearly_inf = 1e6;

struct Fraction {
	int a, b;
	Real val() { return a / b; }
	Fraction operator-() {
		return { -a,b };
	}
};

Fraction evalContinuedFraction(vector<int>&& cf) {
	int n = cf.size();
	if (n == 1)return { cf[0], 1 };
	int p0 = cf[0], q0 = 1, p1 = cf[0] * cf[1] + 1, q1 = cf[1];
	if (n == 2)return { p1, q1 };
	int p2, q2;
	for (int i = 2; i < n; i++) {
		p2 = cf[i] * p1 + p0;
		q2 = cf[i] * q1 + q0;
		p0 = p1; p1 = p2;
		q0 = q1; q1 = q2;
	}
	return { p2, q2 };
}

vector<int> continuedFractionExpansion(Real x) {
	Real target = x;
	vector<int> result; result.reserve(fraction_depth);	
	int wholePart = (int)x;
	result.push_back(wholePart); x = 1 / (x - wholePart);
	for (int i = 1; i < fraction_depth; i++) {
		if (x > nearly_inf)break;
		wholePart = (int)x;
		result.push_back(wholePart); x = 1 / (x - wholePart);
	}
	return result;
}

Fraction floatToFraction(Real x) {
	// If this function is called, it is ensured that x != 0
	if (x > 0) {
		return evalContinuedFraction(continuedFractionExpansion(x));
	}
	else {
		return -evalContinuedFraction(continuedFractionExpansion(-x));
	}
}

VectorXi integerVec(VectorXr& v) {
	int n = v.size();
	vector<Fraction> fracs; fracs.reserve(n);
	for (int i = 0; i < n; i++) {
		fracs.push_back(floatToFraction(v[i]));
	}
	int LCM = fracs[0].b;
	for (int i = 1; i < n; i++) {
		LCM = std::lcm(LCM, fracs[i].b);
	}
	VectorXi vi(n);
	for (int i = 0; i < n; i++) {
		vi[i] = fracs[i].a * (LCM / fracs[i].b);
	}
	return vi;
}