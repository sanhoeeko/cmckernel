#pragma once

#include"base.h"
#include<iostream>

enum FreeEnergy { H, _S, G };
const int Element_num = 48;
const int Number_num = 6;

const string Atom_list[Element_num] = 
{
	"H",	"Li",	"Be",	"B",	"C",	"N",	"O",	"F",	"Ne", 
	"Na",	"Mg",	"Al",	"Si",	"P",	"S",	"Cl",	"K",	"Ca",
	"Ti",	"V",	"Cr",	"Mn",	"Fe",	"Co",	"Ni",	"Cu",	"Zn",
	"As",	"Se",	"Br",	"Sr",	"Y",	"Zr",
	"Nb",	"Mo",	"Ru",	"Rh",	"Pd",	"Ag",	"Cd",	"In",	"Sn",
	"Sb",	"Te",	"I",	"Cs",	"Ba"
};

typedef __base Formula;					// All matters are stored using vectors
typedef vector<Formula> EvalFormula;
typedef uc Card;

inline __base numberCard(uc number) {
	return __base(number + (Element_num - 2), 1);
}

//enum class CardType {
//	Element, Number, Others
//};
//
//inline CardType getCardType(uc cardId) {
//	if (cardId < Element_num) {
//		return CardType::Element;
//	}
//	else if (cardId < Element_num + Number_num) {
//		return CardType::Number;
//	}
//	return CardType::Others;
//}
//
//inline uc getCardIdByName(const string& name) {
//	if (name.size() == 1 && '0' <= name[0] && name[0] <= '9') {
//		return Element_num + stoi(name) - 2;	// constant 2 is fixed
//	}
//	else {
//		for (int i = 0; i < Element_num; i++) {
//			if (Atom_list[i] == name) {
//				return i;
//			}
//		}
//	}
//	throw 114514;
//}
//
//inline string getCardNameById(const uc id) {
//	if (id < Element_num) {
//		return Atom_list[id];
//	}
//	else if (id < Element_num + Number_num) {
//		return to_string(id - Element_num + 2);
//	}
//	else {
//		throw 114519;
//	}
//}
//
//inline vector<EvalFormula> fullCombine(EvalFormula& src, int mul) {
//	int n = src.size();
//	int N = 1 << n;
//	vector<EvalFormula> res; res.reserve(N);
//	for (int i = 1; i < N; i++) {
//		EvalFormula evf; evf.reserve(n);
//		vector<Formula> temp; temp.reserve(n);
//		int combination_repr = i;
//		for (int j = 0; j < n; j++) {
//			if (combination_repr % 2) {
//				evf.push_back(src[j]);
//			}
//			else {
//				temp.push_back(src[j]);
//			}
//			combination_repr = combination_repr >> 1;
//		}
//		EvalFormula evf2; evf2.reserve(n);
//		evf2.push_back(eval(evf, mul));
//		evf2.insert(evf2.end(), temp.begin(), temp.end());
//		res.push_back(evf2);
//	}
//	return res;
//}
//
//inline void splitCardsByType(vector<uc>& cardIds, 
//	vector<uc>& element_cards, vector<uc>& number_cards, vector<uc>& other_cards) {
//	element_cards.clear();
//	number_cards.clear();
//	other_cards.clear();
//	for (auto& id : cardIds) {
//		switch (getCardType(id)) {
//		case CardType::Element:element_cards.push_back(id); break;
//		case CardType::Number:number_cards.push_back(id - Element_num + 2); break;
//		default:other_cards.push_back(id - Element_num - Number_num); break;
//		}
//	}
//}
//
//inline vector<uc> numToNumCardId(vector<uc>& nums) {
//	vector<uc> res; res.reserve(nums.size());
//	for (auto n : nums) {
//		res.push_back(n - 2 + Element_num);
//	}
//	return res;
//}
//
//vector<Formula> getAllPossibleFormulas(vector<uc>& element_cards, vector<uc>& number_cards) {
//	EvalFormula fs; fs.reserve(element_cards.size());
//	for (auto& id : element_cards) {
//		fs.push_back(atom(id));
//	}
//	static vector<EvalFormula> evfs; evfs.clear();
//	static vector<EvalFormula> evfs_buffer; evfs_buffer.clear();
//	evfs.push_back(fs);
//	for (auto& num : number_cards) {
//		evfs_buffer.clear();
//		for (auto& evf : evfs) {
//			auto temps = fullCombine(evf, num);
//			evfs_buffer.insert(evfs_buffer.end(), temps.begin(), temps.end());
//		}
//		std::swap(evfs, evfs_buffer);
//	}
//	vector<Formula> res; res.reserve(evfs.size());
//	for (auto& evf : evfs) {
//		res.push_back(eval(evf));
//	}
//	return res;		// return unique(res); but "==" has not been overloaded
//}
//
///*
//Elems getElementsOnlySingle(Formula& f) {
//	Elems res = 0;
//	int m = f.size();
//	for (int i = 0; i < m; i++) {
//		if (f.nums[i] == 1)res |= (Elems)1 << f.elem[i];
//	}
//	return res;
//}*/
//
//Elems getElements(vector<Card>& cards) {
//	Elems res = 0;
//	for (auto c : cards) {
//		res |= (Elems)1 << c;
//	}
//	return res;
//}
//
//vector<Card> cardFilter(vector<Card>& element_cards, Elems elements) {
//	/*
//		remain if card in elements
//	*/
//	vector<Card> res;
//	for (auto c : element_cards) {
//		if (((Elems)1 << c) & elements)res.push_back(c);
//	}
//	return res;
//}
//vector<Card> cardNegativeFilter(vector<Card>& element_cards, Elems elements) {
//	/*
//		remain if card not in elements
//	*/
//	vector<Card> res;
//	for (auto c : element_cards) {
//		if (!(((Elems)1 << c) & elements))res.push_back(c);
//	}
//	return res;
//}
