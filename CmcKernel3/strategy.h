#pragma once

#include"defs.h"
#include"reaction.h"

struct Advisor{
	MatrixXi can_react_table;
	VectorXi react_abilities;
	Matter* table_start;
	Matter* table_end;
	int num_of_matter;

	Advisor(MatterList& table);
	vector<Matter> getMattersCanReactWith(Matter m);
	int reactAbilityOf(Matter m);
};

struct StrategyList {
	vector<pair<Matter, vector<vector<Card>>>> matter_and_cards;
	int status;

	StrategyList(int statusId);
	void push_back(Matter m, const vector<vector<Card>>& vc);
	int size();
	string toString();
};

struct CardHolder {
	vector<Card> hand;
	vector<Card> element_cards;
	vector<Card> number_cards;
	vector<Card> other_cards;
	Elems elements;
	Advisor* advisor;
	Matter* table_start;
	Matter* table_end;

	CardHolder(Advisor* ptr_advisor);

	/*
	void updateHand(const vector<Card>& cards) {
		hand = cards;
		splitCardsByType(hand, element_cards, number_cards, other_cards);
		elements = getElements(element_cards);
	}
	vector<Matter> availableMatters() {
		vector<Matter> possible_matter = filterMatterCould(elements, table_start, table_end);
		vector<Matter> res;
		for (Matter m : possible_matter) {
			if (!getAllPossibleExpression(m).empty())res.push_back(m);
		}
		return res;
	}
	int score() {
		vector<Matter> ms = availableMatters();
		int s = 0;
		for (Matter m : ms) {
			s += advisor->reactAbilityOf(m);
		}
		return s;
	}*/
	StrategyList getStrategies() {
		/*
			If there is no matter to react
		*/
		/*
		vector<Matter> possible_matter = filterMatterCould(elements, table_start, table_end);
		if (possible_matter.size() == 0)return StrategyList(2);
		StrategyList res(0);
		for (Matter m : possible_matter) {
			res.push_back(m, getAllPossibleExpression(m));
		}
		if (res.size() == 0) {
			res.status = 3;
		}
		return res;*/
	}
	StrategyList getStrategies(Matter m) {
		/*
			get all ways to play facing a Matter m
		*/
		/*
		vector<Matter> ms = advisor->getMattersCanReactWith(m);
		// vector<Matter> ms = calculateMattersCanReactWith(m);
		if (ms.size() == 0)return StrategyList(1);
		vector<Matter> possible_matter = filterMatterCould(elements, ms.data(), ms.data() + ms.size());
		if (possible_matter.size() == 0)return StrategyList(2);
		StrategyList res(0);
		for (Matter m : possible_matter) {
			res.push_back(m, getAllPossibleExpression(m));
		}
		if (res.size() == 0) {
			res.status = 3;
		}
		return res;*/
	}
};
