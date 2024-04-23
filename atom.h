#pragma once

#include"base.h"
#include<iostream>

const int Element_num = 64;
const int Number_num = 6;

const string Atom_list[Element_num] = {"[NOP]",
	"H",	"He",	"Li",	"Be",	"B",	"C",	"N",	"O",	"F",	"Ne", 
	"Na",	"Mg",	"Al",	"Si",	"P",	"S",	"Cl",	"Ar",	"K",	"Ca",
	"Sc",	"Ti",	"V",	"Cr",	"Mn",	"Fe",	"Co",	"Ne",	"Cu",	"Zn",
	"Ga",	"Ge",	"As",	"Se",	"Br",	"Kr",	"Rb",	"Sr",	"Y",	"Zr",
	"Nb",	"Mo",	"Tc",	"Ru",	"Rh",	"Pd",	"Ag",	"Cd",	"In",	"Sn",
	"Sb",	"Te",	"I",	"Xe",	"Cs",	"Ba"
};

typedef __base Formula;					// All matters are stored using vectors
typedef vector<Formula> EvalFormula;
typedef uc Card;

enum class CardType {
	Element, Number, Others
};

inline CardType getCardType(int cardId) {
	if (cardId < Element_num) {
		return CardType::Element;
	}
	else if (cardId < Element_num + Number_num) {
		return CardType::Number;
	}
	return CardType::Others;
}

inline Formula zero() {
	return Formula();
}
inline Formula atom(uc id) {
	return Formula(id, 1);
}
inline Formula eval(EvalFormula& evf) {
	Formula z = zero();
	z.sum(evf);
	return z;
}
inline Formula eval(EvalFormula& evf, int mul) {
	Formula z = zero();
	z.sum(evf); z *= mul;
	return z;
}

inline string to_string(Formula& f) {
	string s = "";
	int m = f.size();
	for (int i = 0; i < m; i++) {
		s += "(" + Atom_list[f.elem[i]] + "," + to_string(f.nums[i]) + ")";
	}
	return s;
}

template<typename ty>
vector<ty> unique(vector<ty>& src) {
	vector<ty> res;
	bool flag = 0;
	for (auto& i : src) {
		flag = 0;
		for (auto& j : res) {
			if (i == j) {
				flag = 1; break;
			}
		}
		if(!flag) res.push_back(i);
	}
	return res;
}

inline vector<EvalFormula> fullCombine(EvalFormula& src, int mul) {
	int n = src.size();
	int N = 1 << n;
	vector<EvalFormula> res; res.reserve(N);
	for (int i = 1; i < N; i++) {
		EvalFormula evf;
		vector<Formula> temp; temp.reserve(n);
		int combination_repr = i;
		for (int j = 0; j < n; j++) {
			if (combination_repr % 2) {
				evf.push_back(src[j]);
			}
			else {
				temp.push_back(src[j]);
			}
			combination_repr = combination_repr >> 1;
		}
		EvalFormula evf2;
		evf2.push_back(eval(evf, mul));
		evf2.insert(evf2.end(), temp.begin(), temp.end());
		res.push_back(evf2);
	}
	return res;
}

inline void splitCardsByType(vector<uc>& cardIds, 
	vector<uc>& element_cards, vector<uc>& number_cards, vector<uc>& other_cards) {
	for (auto& id : cardIds) {
		switch (getCardType(id)) {
		case CardType::Element:element_cards.push_back(id); break;
		case CardType::Number:number_cards.push_back(id - Element_num + 2); break;
		default:other_cards.push_back(id - Element_num - Number_num); break;
		}
	}
}

vector<Formula> geoAllPossibleFormulas(vector<uc>& cardIds) {
	vector<uc> element_cards;
	vector<uc> number_cards;
	vector<uc> other_cards;
	splitCardsByType(cardIds, element_cards, number_cards, other_cards);
	EvalFormula fs; fs.reserve(element_cards.size());
	for (auto& id : element_cards) {
		fs.push_back(atom(id));
	}
	vector<EvalFormula>* evfs = new vector<EvalFormula>();
	vector<EvalFormula>* evfs_buffer = new vector<EvalFormula>();
	evfs->push_back(fs);
	for (auto& num : number_cards) {
		evfs_buffer->clear();
		for (auto& evf : *evfs) {
			auto temps = fullCombine(evf, num);
			evfs_buffer->insert(evfs_buffer->end(), temps.begin(), temps.end());
		}
		std::swap(evfs, evfs_buffer);
	}
	vector<Formula> res; res.reserve(evfs->size());
	for (auto& evf : *evfs) {
		res.push_back(eval(evf));
	}
	delete evfs; delete evfs_buffer;
	return res;		// return unique(res); but "==" has not been overloaded
}

bits getElements(Formula& f) {
	return f.getElements();
}
bits getElementsOnlySingle(Formula& f) {
	bits res = 0;
	int m = f.size();
	for (int i = 0; i < m; i++) {
		if (f.nums[i] == 1)res &= (bits)1 << f.elem[i];
	}
	return res;
}
bits getElements(vector<Card>& cards) {
	bits res = 0;
	for (auto c : cards) {
		res |= (bits)1 << c;
	}
	return res;
}

vector<Card> cardFilter(vector<Card>& element_cards, bits elements) {
	/*
		remain if card in elements
	*/
	vector<Card> res;
	for (auto c : element_cards) {
		if (c & elements)res.push_back(c);
	}
	return res;
}
vector<Card> cardNegativeFilter(vector<Card>& element_cards, bits elements) {
	/*
		remain if card not in elements
	*/
	vector<Card> res;
	for (auto c : element_cards) {
		if (!(c & elements))res.push_back(c);
	}
	return res;
}

typedef vector<Card> Strategy;

vector<Strategy> getAllPossibleStrategy(Formula& f, vector<Card>& hand) {
	vector<Strategy> res;
	bits required_elements = getElements(f);
	vector<Card> element_cards;
	vector<Card> number_cards;
	vector<Card> other_cards;
	splitCardsByType(hand, element_cards, number_cards, other_cards);
	// find considered element cards
	vector<Card> useful_element_cards = cardFilter(element_cards, required_elements);
	bits has_elements = getElements(useful_element_cards);
	if (has_elements != required_elements) {
		return res;		// return an empty list of strategies
	}
	// find elements that has only one atom. They are not related to number cards.
	bits single_elements = getElementsOnlySingle(f);
	vector<Card> strategic_element_cards = cardNegativeFilter(useful_element_cards, single_elements);
	// get all possible combinations (and meanings) of number cards and strategic element cards

}

