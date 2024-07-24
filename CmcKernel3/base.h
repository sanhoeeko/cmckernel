#pragma once

#include"defs.h"
#include<stdlib.h>
#include<string>
#include<vector>
using namespace std;

typedef unsigned char uc;
typedef unsigned __int64 uc8;
typedef unsigned __int64 Elems;		// It requires at most 63 elements.

const int basic_num_chars = 64;
const int longs = basic_num_chars / 8;

struct __base
{
	uc data[basic_num_chars];

	__base();
	__base(uc id, uc num);
	__base(const Elems& elems);
	
	bool operator==(const __base& o);
	bool operator!=(const __base& o);
	bool operator<(const __base& o);
	bool isZero();
	__base operator+=(const __base& o);
	__base operator*=(const uc mul);
	__base operator*(const uc mul);
	__base operator+(const __base& o) const;
	__base operator-(const __base& o) const;
	__base sum(vector<__base>& vb);
	__base unityPart() const;
	__int64 compress(vector<uc>& indices);
	Elems getElements();
	json toJson();
	string toString();
};

inline __base zero() {
	return __base();
}
inline __base atom(uc id) {
	return __base(id, 1);
}

inline __base eval(vector<__base>& evf) {
	return zero().sum(evf);
}
inline __base eval(vector<__base>& evf, uc mul) {
	return eval(evf) *= mul;
}

/*
vector<uc> to_vector(Elems elems) {
	vector<uc> res; res.reserve(8);
	uc cnt = 0;
	while (elems = elems >> 1) {
		cnt++;
		if (elems & 1)res.push_back(cnt);
	}
	return res;
}

Elems VecToElems(vector<uc>&& vec) {
	Elems res = 0;
	for (uc e : vec) {
		res |= (Elems)1 << e;
	}
	return res;
}*/