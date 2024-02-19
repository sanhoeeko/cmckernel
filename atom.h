#pragma once

#include<string>
#include<vector>
#include<set>
#include<iostream>
#include<Eigen/Sparse>
using namespace std;

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

typedef Eigen::SparseMatrix<int> Formula;					// All matters are stored using vectors
typedef Eigen::SparseMatrix<int>::InnerIterator FormulaIter;
typedef vector<Formula> EvalFormula;
typedef int Card;

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

void printSparse(Formula& f) {
	auto mat = f.toDense();
	int* p = mat.data();
	int* pend = p + Element_num;
	int cnt = 0;
	for (; p < pend; p++) {
		cout << *p << ", "; cnt++;
		if (cnt == 16) {
			cnt = 0; cout << endl;
		}
	}
}

inline Formula zero() {
	Formula m(Element_num, 1); return m;	// note that the storage is column first
}
inline Formula atom(int id) {
	Formula m(Element_num, 1); m.insert(id, 0) = 1; return m;
}
inline Formula eval(EvalFormula& evf) {
	Formula res = zero();
	for (auto& m : evf) {
		res += m;
	}
	return res;
}
inline Formula eval(EvalFormula& evf, int mul) {
	Formula res = zero();
	for (auto& m : evf) {
		res += m;
	}
	return res * mul;
}

inline string to_string(Formula& f) {
	string s = "";
	for (FormulaIter it(f, 0); it; ++it) {
		int i = it.index();
		s += "(" + Atom_list[i] + "," + to_string(it.value()) + ")";
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

inline void splitCardsByType(vector<int>& cardIds, 
	vector<int>& element_cards, vector<int>& number_cards, vector<int>& other_cards) {
	for (auto& id : cardIds) {
		switch (getCardType(id)) {
		case CardType::Element:element_cards.push_back(id); break;
		case CardType::Number:number_cards.push_back(id - Element_num + 2); break;
		default:other_cards.push_back(id - Element_num - Number_num); break;
		}
	}
}

vector<Formula> geoAllPossibleFormulas(vector<int>& cardIds) {
	vector<int> element_cards;
	vector<int> number_cards;
	vector<int> other_cards;
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

set<int> getElements(Formula& f) {
	set<int> res;
	for (FormulaIter it(f, 0); it; ++it) {
		res.insert(it.index());
	}
	return res;
}
set<int> getElements(vector<Card> cards) {
	vector<int> vint = unique(cards);
	set<int> res(vint.begin(), vint.end());
	return res;
}

vector<Card> filter(vector<Card>& element_cards, set<int>& elements) {
	vector<Card> res;
	for (auto i : element_cards) {
		for (auto j : elements) {
			if (i == j) {
				res.push_back(i); break;
			}
		}
	}
	return res;
}

typedef vector<Card> Strategy;

vector<Strategy> getAllPossibleStrategy(Formula& f, vector<Card>& hand) {
	vector<Strategy> res;
	set<int> required_elements = getElements(f);
	vector<Card> element_cards;
	vector<Card> number_cards;
	vector<Card> other_cards;
	splitCardsByType(hand, element_cards, number_cards, other_cards);
	vector<Card> useful_element_cards = filter(element_cards, required_elements);
	set<int> has_elements = getElements(useful_element_cards);
	if (has_elements != required_elements) {
		return res;		// return an empty list of strategies
	}
}

