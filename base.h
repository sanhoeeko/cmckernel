#pragma once

#include"defs.h"
#include<stdlib.h>
#include<string>
#include<vector>
#include<memory>
using namespace std;

typedef unsigned char uc;
typedef unsigned __int64 uc8;
typedef unsigned __int64 bits;	// It requires at most 63 elements.
const int basesz = 8;			// In fact, the maximum number of element is 7. [elem] need a zero at its end.

/*
	We do not use std::set, but use [bits], with std::vector as an alternative representation of sets.
*/
vector<uc> bitsToVec(bits elems) {
	vector<uc> res; res.reserve(basesz);
	uc cnt = 0;
	while (elems = elems >> 1) {
		cnt++;
		if (elems & 1)res.push_back(cnt);
	}
	return res;
}
bits vecToBits(vector<uc>&& vec) {
	bits res = 0;
	for (uc e : vec) {
		res |= (bits)1 << e;
	}
	return res;
}

struct __base
{
	uc elem[basesz];
	uc nums[basesz];

	__base() : elem{ 0 }, nums{ 0 } {
		;
	}
	__base(uc key, uc val) : elem { 0 }, nums{ 0 } {
		elem[0] = key; nums[0] = val;
	}
	string toString() {
		uc* ptr = elem;
		string res;
		while (*ptr != 0) {
			res += "(" + to_string((int)(*ptr)) + "," + to_string((int)ptr[basesz]) + ")";
			ptr++;
		}
		return res;
	}
	void operator*=(int a) {
		uc8* p = reinterpret_cast<uc8*>(nums);		// it requires basesz = 8.
		*p *= a;
	}
	__base operator*(int a) {
		__base res = *this;
		res *= a;
		return res;
	}
	int size() {
		int cnt = 0;
		uc* ptr = elem;
		while (*ptr++ != 0)cnt++;
		return cnt;
	}
	void _insert(int m, uc key, uc val) {
		elem[m] = key;
		nums[m] = val;
	}
	void _add(int m, uc val) {
		nums[m] += val;
	}
	int _index(uc key) {
		uc* ptr = elem;
		int cnt = 0;
		while (*ptr != 0) {
			if (*ptr++ == key) {
				return cnt;
			}cnt++;
		}
		return -1;
	}
	void _direct_add(__base& y) {
		int m = size();
		uc* ptr = y.elem;
		while (*ptr != 0){
			int idx = _index(*ptr);
			if (idx == -1) {
				_insert(m++, *ptr, ptr[basesz]);
			}
			else {
				_add(idx, ptr[basesz]);
			}
			ptr++;
		}
	}
	void _sort() {	// fast sort for small array
		int m = size();
		for (int i = 1; i < m; ++i)
		{
			unsigned char key_elem = elem[i];
			unsigned char key_nums = nums[i];
			int j = i - 1;

			while (j >= 0 && elem[j] > key_elem)
			{
				elem[j + 1] = elem[j];
				nums[j + 1] = nums[j];
				--j;
			}

			elem[j + 1] = key_elem;
			nums[j + 1] = key_nums;
		}
	}
	void sum(vector<__base>& vec) {
		for (auto& y : vec) {
			_direct_add(y);
		}
		_sort();
	}
	__base operator+(__base& y) {
		__base res;
		uc* px = this->elem;
		uc* py = y.elem;
		uc* pz = res.elem;
		while (*px != 0 && *py != 0) {
			if (*px < *py) {
				*pz = *px; pz[basesz] = px[basesz];
				pz++; px++;
			}
			else if (*px == *py) {
				*pz = *px; pz[basesz] = px[basesz] + py[basesz];
				pz++; px++; py++;
			}
			else {
				*pz = *py; pz[basesz] = py[basesz];
				pz++; py++;
			}
		}
		if (*px == 0) {
			while (*py != 0) {
				*pz = *py; pz[basesz] = py[basesz];
				pz++; py++;
			}
		}
		else {
			while (*px != 0){
				*pz = *px; pz[basesz] = px[basesz];
				pz++; px++;
			}
		}
		return res;
	}
	void operator+=(__base& y) {
		*this = *this + y;
	}
	bool operator==(__base& y) {
		return *(uc8*)this->nums == *(uc8*)y.nums && *(uc8*)this->elem == *(uc8*)y.elem;
	}
	bool operator!=(__base& y) {
		return *(uc8*)this->nums != *(uc8*)y.nums || *(uc8*)this->elem != *(uc8*)y.elem;
	}
	bits getElements() {
		bits res = 0;
		uc* ptr = elem;
		while (*ptr != 0) {
			res |= (bits)1 << *ptr++;
		}
		return res;
	}
	template<typename dtype>
	void cast_to_elements(vector<uc>& elements, dtype* dst) {
		uc* ptr = elem;
		for (auto e : elements) {
			if (*ptr == e) {
				*dst++ = ptr[basesz]; ptr++;
			}
			else {
				*dst++ = 0;
			}
		}
	}
};