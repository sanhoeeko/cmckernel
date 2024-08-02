#pragma once

#include"defs.h"
#include<vector>
#include<stack>
#include<map>
using namespace std;

/*
	All functions here are templates
*/

template<typename t>
inline bool in(const t& x, const vector<t>& v) {
	for (auto& y : v) {
		if (x == y)return true;
	}
	return false;
}


template<typename ty>
vector<ty> Unique(vector<ty>& src) {
	vector<ty> res; res.reserve(src.size());
	bool flag = 0;
	for (auto& i : src) {
		if (!in(i, res)) res.push_back(i);
	}
	return res;
}

template <typename T>
void operator+=(vector<T>& v1, const vector<T>& v2)
{
	v1.insert(v1.end(), v2.begin(), v2.end());
}

template <typename T>
vector<T> operator+(const vector<T>& v1, const vector<T>& v2)	// [const] ensures structures like [v1 + v2 + v3]
{
	vector<T> res = v1;
	res.insert(res.end(), v2.begin(), v2.end());
	return res;
}

template <typename ty>
string to_string(const vector<ty>& vec) {
	string res = "[";
	for (auto& x : vec) {
		res += to_string(x) + ",";
	}
	return res + "]";
}
