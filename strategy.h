#pragma once

#include"defs.h"
#include"reaction.h"
#include"io.h"

struct Advisor{
	MatrixXi can_react_table;
	int num_of_matter;

	Advisor() {
		vector<int> temp;
		int can_react_mat_size = readCanReactMatrix("can_react.matrix", temp);
		this->can_react_table = MatrixXi(can_react_mat_size, can_react_mat_size);
		this->can_react_table.setZero();
		int length = can_react_mat_size - 1;
		int* ptr = temp.data();
		for (int i = 0; i < can_react_mat_size - 1; i++) {
			memcpy(this->can_react_table.data() + can_react_mat_size * i + (i + 1), ptr, length * sizeof(int));
			ptr += length;
			length--;
		}
		this->can_react_table += this->can_react_table.transpose().eval();
		this->num_of_matter = can_react_mat_size;
	}

	vector<Matter> getMattersCanReactWith(Matter m) {
		vector<Matter> res; res.reserve(num_of_matter);
		int* ptr = can_react_table.data() + m->id * num_of_matter;
		for (int i = 0; i < num_of_matter; i++) {
			if (ptr[i] == 1) {
				res.push_back(table[i]);
			}
		}
		return res;
	}
};

string strategy_status[16] = {
	"Success",
	"Terminal matter on the field",
	"I need element cards",
};

struct StrategyList {
	vector<pair<Matter, vector<vector<Card>>>> matter_and_cards;
	int status;

	StrategyList(int statusId) {
		status = statusId;
	}
	void push_back(Matter m, const vector<vector<Card>>& vc) {
		if (vc.size() == 0)return;
		matter_and_cards.push_back(make_pair(m, vc));
	}
	string toString() {
		string str = strategy_status[status] + "; ";
		for (auto p : matter_and_cards) {
			str += to_string(p.first) + ": " + to_string(p.second) + "\n";
		}
		return str;
	}
};

string to_string(StrategyList& stl) {
	return stl.toString();
}

struct CardHolder {
	vector<Card> hand;
	vector<Card> element_cards;
	vector<Card> number_cards;
	vector<Card> other_cards;
	bits elements;
	Advisor* advisor;

	CardHolder() {
		advisor = new Advisor();
	}
	void updateHand(vector<Card>& cards) {
		hand = cards;
		splitCardsByType(hand, element_cards, number_cards, other_cards);
		elements = getElements(element_cards);
	}
	StrategyList getStrategies(Matter m) {
		/*
			get all ways to play facing a Matter m
		*/
		vector<Matter> ms = advisor->getMattersCanReactWith(m);
		if (ms.size() == 0)return StrategyList(1);
		vector<Matter> possible_matter = filterMatterCould(elements, ms.data(), ms.data() + ms.size());
		if (possible_matter.size() == 0)return StrategyList(2);
		StrategyList res(0);
		for (Matter m : possible_matter) {
			res.push_back(m, getAllPossibleExpression(m));
		}
		return res;
	}
	vector<vector<Card>> getAllPossibleExpression(Matter m) {
		/*
			get all ways to express a formula (of a matter) using cards in hand
			elements have been checked before this
		*/
		vector<vector<Card>> res;
		// find considered element cards
		vector<Card> useful_element_cards = cardFilter(element_cards, m->elems);
		// find elements that has only one atom. They are not related to number cards.
		bits single_elements = getElementsOnlySingle(m->formula);
		vector<Card> required_element_cards = bitsToVec(single_elements);
		vector<Card> strategic_element_cards = cardNegativeFilter(useful_element_cards, single_elements);
		Formula target = m->formula.exclude(single_elements);
		bits target_elements = m->elems - single_elements;
		// get all possible combinations (and meanings) of number cards and strategic element cards
		auto it = SubsetIterator(&strategic_element_cards);
		while (it.goes()){
			vector<Card>* element_subset = it.next();
			if (getElements(*element_subset) != target_elements) {
				continue;
			}
			auto jt = SubsetIterator(&number_cards);
			while (jt.goes()) {
				vector<Card>* number_subset = jt.next();
				vector<Formula> combinations = getAllPossibleFormulas(*element_subset, *number_subset);
				for (Formula& f : combinations) {
					if (f == target) {
						// if the element subset and the number subset can make this formula, record them
						vector<Card> vc = required_element_cards + *element_subset + numToNumCardId(*number_subset);
						sort(vc.begin(), vc.end());
						if (!in(vc, res)) {
							res.push_back(vc);
						}
					}
				}
			}
		}
		return res;
	}
};

void checkCanReactMatrix(Matter a) {
	Advisor advisor;
	vector<Matter> ms = advisor.getMattersCanReactWith(a);
	for (Matter m : table) {
		cout << to_string(m);
		if (canReact(m, a)) {
			if (in(m, ms)) {
				cout << " ." << endl;	// normal
			}
			else {
				cout << " o" << endl;	// should be "can react"
			}
		}
		else {
			if (std::find(ms.begin(), ms.end(), m) != ms.end()) {
				cout << " x" << endl;	// should be "cannot react"
			}
			else{
				cout << " ." << endl;	// normal
			}
		}
	}
}