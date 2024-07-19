// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include<string>
using namespace std;

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

#include"global.h"

const char* ctypesWrapper(string f(string&), const char* input) {
	static string input_buffer;
	static string output_buffer;
	try {
		input_buffer = input;
		output_buffer = f(input_buffer);
		return output_buffer.c_str();
	}
	catch (const std::exception& e) {
		cout << "Exception" << e.what() << endl;
	}
	catch (int exception) {
		cout << "Exception code: " << exception << endl;
		cout << "Bad parameter when calling dll. receiving: " << input_buffer << endl;
		return "";
	}
}

string _cardNamesToIds(string& names) {
	auto sp = Split(names);
	string res;
	while (sp.goes())
	{
		res += to_string((int)getCardIdByName(sp.next(','))) + ",";
	}
	return res;
}

string _cardIdsToNames(string& ids) {
	auto sp = Split(ids);
	string res;
	while (sp.goes())
	{
		res += getCardNameById(stoi(sp.next(','))) + ",";
	}
	return res;
}

string _queryLegalStrategies(string& matter_and_cardIds)
{
	auto sp = Split(matter_and_cardIds);
	string matter_name = sp.next(';');
	kernel->hand->updateHand(parseCardIds(sp.next(';')));
	return kernel->hand->getStrategies(kernel->getMatterByName(matter_name)).toString();
}

string _queryStrategyToBegin(string& cardIds) {
	kernel->hand->updateHand(parseCardIds(cardIds));
	return kernel->hand->getStrategies().toString();
}

string _queryPossibleMatters(string& cardsStr) {
	auto sp = Split(cardsStr);
	vector<uc> cards;
	while (sp.goes())
	{
		cards.push_back(getCardIdByName(sp.next(' ')));
	}
	vector<Matter> possible_matters = getAllPossibleMatters(cards, kernel->table_start, kernel->table_end);
	return to_string(possible_matters);
}

string _queryReactions(string& matter_pair) {
	auto sp = Split(matter_pair);
	Matter a = kernel->getMatterByName(sp.next(','));
	Matter b = kernel->getMatterByName(sp.next(','));
	if (a == NULL || b == NULL) {
		throw 114521;
	}
	vector<Reaction> rs = kernel->getPossibleReactions(a, b);
	return to_string(rs);
}

string _queryWhichHasMinH(string& matters) {
	auto sp = Split(matters);
	float h_min = FLT_MAX;
	Matter current_matter = NULL;
	while (sp.goes())
	{
		Matter m = kernel->getMatterByName(sp.next(','));
		if (m == NULL) throw 114520;
		if (m->g(G) < h_min) {
			h_min = m->g(G); current_matter = m;
		}
	}
	return to_string(current_matter);
}

string _showAllMattersHasElement(string& element) {
	uc elem = getCardIdByName(element);
	bits elems = (bits)1 << elem;
	vector<Matter> ms = filterMatterShould(elems, kernel->table_start, kernel->table_end);
	return to_string(ms);
}

string _showAllCanReactWith(string& matter) {
	vector<Matter> ms = kernel->calculateMattersCanReactWith(kernel->getMatterByName(matter));
	return to_string(ms);
}

string _queryScoreOfCards(string& cardIds) {
	kernel->hand->updateHand(parseCardIds(cardIds));
	return to_string(kernel->hand->score());
}


// Interfaces

void Init() {
	kernel = new CMCKernel();
}

void GenerateCanReactMatrix() {
	kernel->GenerateCanReactMatrix("canReact.dat");
}

const char* CardNamesToIds(char* x) {
	return ctypesWrapper(_cardNamesToIds, x);
}

const char* CardIdsToNames(char* x) {
	return ctypesWrapper(_cardIdsToNames, x);
}

const char* QueryLegalStrategies(char* x) {
	return ctypesWrapper(_queryLegalStrategies, x);
}

const char* QueryPossibleMatters(char* x) {
	return ctypesWrapper(_queryPossibleMatters, x);
}

const char* QueryReactions(char* x) {
	return ctypesWrapper(_queryReactions, x);
}

const char* QueryWhichHasMinH(char* x) {
	return ctypesWrapper(_queryWhichHasMinH, x);
}

const char* ShowAllMattersHasElement(char* x) {
	return ctypesWrapper(_showAllMattersHasElement, x);
}

const char* ShowAllCanReactWith(char* x) {
	return ctypesWrapper(_showAllCanReactWith, x);
}

const char* QueryStrategyToBegin(char* x) {
	return ctypesWrapper(_queryStrategyToBegin, x);
}

const char* QueryScoreOfCards(char* x) {
	return ctypesWrapper(_queryScoreOfCards, x);
}