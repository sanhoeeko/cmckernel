#pragma once

#include"defs.h"
#include<stdlib.h>
#include<string>
#include<vector>
using namespace std;

typedef unsigned char uc;
typedef unsigned __int64 uc8;
typedef unsigned __int64 Elems;		// It can express at most 63 elements.

const int basic_num_chars = 64;
const int longs = basic_num_chars / 8;

struct __base
{
	uc data[basic_num_chars];

	__base();
	__base(uc id, uc num);
	__base(const Elems& elems);
	
	bool operator==(const __base& o) const;
	bool operator!=(const __base& o) const;
	bool operator<(const __base& o) const;
	bool isZero();
	__base operator+=(const __base& o);
	__base operator*=(const uc mul);
	__base operator*(const uc mul);
	__base operator+(const __base& o) const;
	__base operator-(const __base& o) const;
	__base sum(vector<__base>& vb);
	__base unityPart() const;
	uc8 compress(vector<uc>& indices);
	uc count() const;
	Elems getElements() const;
	json toJson() const;
	string toString() const;
	template<typename ty> void cast_to_elements(vector<uc>& indices, ty* ptr) {
		for (int i = 0; i < indices.size(); i++) {
			ptr[i] = data[indices[i]];
		}
	}
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

string to_string(const __base& b);
vector<uc> to_vector(Elems elems);
__base from_vector(const vector<vector<uc>>& vec);
__base from_cardIds(const vector<int>& vec);