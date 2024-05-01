#pragma once

#include"defs.h"
#include<vector>
using namespace std;

template<typename ty>
inline bool in(ty& item, vector<ty>& v) {
	return std::find(v.begin(), v.end(), item) != v.end();
}

template<typename ty>
vector<ty> unique(vector<ty>& src) {
	vector<ty> res;
	bool flag = 0;
	for (auto& i : src) {
		if (!in(i, res)) res.push_back(i);
	}
	return res;
}

template<typename ty>
struct SubsetIterator {
	vector<ty>* pv;
	vector<ty>* temp;
	int full_size;
	int cnt;
	int max_cnt;

	SubsetIterator(vector<ty>* v_ptr, int start=0) {
		pv = v_ptr;
		full_size = pv->size();
		temp = new vector<ty>(); temp->reserve(full_size);
		cnt = start;
		max_cnt = 1 << full_size;
	}
	vector<ty>* next() {
		temp->clear();
		int num = cnt++;
		for(int idx = 0; idx < full_size; idx++)
		{
			if (num % 2)temp->push_back((*pv)[idx]);
			num = num >> 1;
		}
		return temp;
	}
	bool goes() {
		return cnt < max_cnt;
	}
};

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
string to_string(vector<ty>& vec) {
	string res = "[";
	for (auto& x : vec) {
		res += to_string(x) + ",";
	}
	return res + "]";
}