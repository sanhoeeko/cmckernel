#include "pch.h"
#include "strategy.h"
#include "utils.h"

string strategy_status[16] = {
	"Success",
	"Terminal matter on the field",
	"I need element cards",
	"I need auxiliary cards"
};

Advisor::Advisor(MatterList& table)
{
	vector<int> temp;
	int can_react_mat_size = readCanReactMatrix("canReact.dat", temp);
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
	this->react_abilities = can_react_table.rowwise().sum();
	this->num_of_matter = can_react_mat_size;
}

vector<Matter> Advisor::getMattersCanReactWith(Matter m)
{
	vector<Matter> res; res.reserve(num_of_matter);
	int* ptr = can_react_table.data() + m->id * num_of_matter;
	for (int i = 0; i < num_of_matter; i++) {
		if (ptr[i] == 1) {
			res.push_back(table_start[i]);
		}
	}
	return res;
}

int Advisor::reactAbilityOf(Matter m)
{
	return react_abilities[m->id];
}

StrategyList::StrategyList(int statusId)
{
	status = statusId;
}

void StrategyList::push_back(Matter m, const vector<vector<Card>>& vc)
{
	if (vc.size() == 0)return;
	matter_and_cards.push_back(make_pair(m, vc));
}

int StrategyList::size()
{
	return matter_and_cards.size();
}

string StrategyList::toString()
{
	string str = strategy_status[status] + "; ";
	for (auto p : matter_and_cards) {
		str += p.first->toString() + ": " + to_string(p.second) + "\n";
	}
	return str;
}

CardHolder::CardHolder(Advisor* ptr_advisor)
{
	advisor = ptr_advisor;
	table_start = advisor->table_start;
	table_end = advisor->table_end;
}