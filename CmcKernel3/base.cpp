#include "pch.h"
#include "base.h"

template<int n>
inline static uc byteExists(uc8 x) {
	return (uc)((x & ((uc8)0xff << (8 * n))) != 0) << n;
}

template<int n>
inline static uc8 bitExtract(char x) {
	return (uc8)(x & ((uc)1 << n)) << (n * 8);
}

inline static uc summarize(uc8 x) {
	return
		byteExists<0>(x) | byteExists<1>(x) | byteExists<2>(x) | byteExists<3>(x) |
		byteExists<4>(x) | byteExists<5>(x) | byteExists<6>(x) | byteExists<7>(x);
}

inline static uc8 cast64(char x) {
	return
		bitExtract<0>(x) | bitExtract<1>(x) | bitExtract<2>(x) | bitExtract<3>(x) |
		bitExtract<4>(x) | bitExtract<5>(x) | bitExtract<6>(x) | bitExtract<7>(x);
}

__base::__base()
{
	memset(data, 0, basic_num_chars * sizeof(char));
}

__base::__base(uc id, uc num) : __base()
{
	data[id] = num;
}

__base::__base(const Elems& elems)
{
	uc8* ptr = (uc8*)data;
	char* ref = (char*)&elems;
	for (int i = 0; i < longs; i++) {
		ptr[i] = cast64(ref[i]);
	}
}

bool __base::operator==(const __base& o) const
{
	uc8* p = (uc8*)data;
	uc8* q = (uc8*)o.data;
	return
		p[0] == q[0] && p[1] == q[1] && p[2] == q[2] && p[3] == q[3] &&
		p[4] == q[4] && p[5] == q[5] && p[6] == q[6] && p[7] == q[7];
}

bool __base::operator!=(const __base& o) const
{
	return !(*this == o);
}

bool __base::operator<(const __base& o) const
{
	uc8* p = (uc8*)data;
	uc8* q = (uc8*)o.data;
	for (int i = 0; i < longs; i++) {
		if (p[i] > q[i]) return false;
		if (p[i] < q[i]) return true;
	}
	return false;
}

bool __base::isZero()
{
	uc8* ptr = (uc8*)data;
	return
		ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0 &&
		ptr[4] == 0 && ptr[5] == 0 && ptr[6] == 0 && ptr[7] == 0;
}

__base __base::operator+=(const __base& o)
{
	uc8* p = (uc8*)data;
	uc8* q = (uc8*)o.data;
	for (int i = 0; i < longs; i++) {
		p[i] += q[i];
	}
	return *this;
}

__base __base::operator*=(uc mul)
{
	uc8* ptr = (uc8*)data;
	for (int i = 0; i < longs; i++) {
		ptr[i] *= mul;
	}
	return *this;
}

__base __base::operator*(const uc mul)
{
	__base res;
	uc8* ptr = (uc8*)data;
	uc8* res_ptr = (uc8*)res.data;
	for (int i = 0; i < longs; i++) {
		res_ptr[i] *= mul;
	}
	return res;
}

__base __base::operator+(const __base& o) const
{
	__base res;
	uc8* p = (uc8*)data;
	uc8* q = (uc8*)o.data;
	uc8* r = (uc8*)res.data;
	for (int i = 0; i < longs; i++) {
		r[i] = p[i] + q[i];
	}
	return res;
}

__base __base::operator-(const __base& o) const
{
	__base res;
	uc8* p = (uc8*)data;
	uc8* q = (uc8*)o.data;
	uc8* r = (uc8*)res.data;
	for (int i = 0; i < longs; i++) {
		r[i] = p[i] - q[i];
	}
	return res;
}

Elems __base::getElements() const
{
	uc8 res = 0;
	char* res_ptr = (char*)&res;
	uc8* ptr = (uc8*)data;
	for (int i = 0; i < longs; i++) {
		res_ptr[i] = summarize(ptr[i]);
	}
	return res;
}

json __base::toJson() const
{
	json js;
	for (int i = 0; i < basic_num_chars; i++) {
		if (data[i] != 0) {
			js.push_back({ i, data[i] });
		}
	}
	return js;
}

string __base::toString() const
{
	return toJson().dump();
}

__base __base::sum(vector<__base>& vb)
{
	for (auto& b : vb) *this += b;
	return *this;
}

/*
	return a set of elements, each of which has only one atom
*/
__base __base::unityPart() const
{
	__base res;
	for (int i = 0; i < basic_num_chars; i++) {
		if (data[i] == 1) {
			res.data[i] = 1;
		}
	}
	return res;
}

/*
	the number of elements in __base should be less than 8
	require: `indices` is empty
*/
uc8 __base::compress(vector<uc>& indices)
{
	uc8 res = 0;
	uc* ptr = (uc*)&res;
	int cnt = 0;
	indices.reserve(8);
	for (int i = 0; i < basic_num_chars; i++) {
		if (data[i] != 0) {
			indices.push_back(i);
			ptr[cnt++] = data[i];
		}
	}
	return res;
}

uc __base::count() const
{
	uc cnt = 0;
	for (int i = 0; i < basic_num_chars; i++) {
		cnt += data[i];
	}
	return cnt;
}

string to_string(const __base& b)
{
	return b.toString();
}

vector<uc> to_vector(Elems elems) {
	vector<uc> res; res.reserve(8);
	uc cnt = 0;
	while (elems = elems >> 1) {
		cnt++;
		if (elems & 1)res.push_back(cnt);
	}
	return res;
}

__base from_vector(const vector<vector<uc>>& vec)
{
	__base res;
	for (auto& x : vec) {
		res.data[x[0]] = x[1];
	}
	return res;
}

__base from_cardIds(const vector<int>& vec) {
	__base res;
	for (auto cardId : vec) {
		res.data[cardId]++;
	}
	return res;
}