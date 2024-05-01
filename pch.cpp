// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include<string>
using namespace std;

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

#include"strategy.h"

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
	static CardHolder ch;
	auto sp = Split(matter_and_cardIds);
	string matter_name = sp.next(';');
	vector<uc> cid;
	int id;
	while (sp.goes()) {
		try {
			id = stoi(sp.next(','));
		}
		catch (std::invalid_argument&) {
			throw 114518;
		}
		cid.push_back(id);
	}
	ch.updateHand(cid);
	return ch.getStrategies(getMatterByName(matter_name)).toString();
}

string _queryPossibleMatters(string& cardsStr) {
	auto sp = Split(cardsStr);
	vector<uc> cards;
	while (sp.goes())
	{
		cards.push_back(getCardIdByName(sp.next(' ')));
	}
	vector<Matter> possible_matters = getAllPossibleMatters(cards);
	return to_string(possible_matters);
}

string _queryReactions(string& matter_pair) {
	auto sp = Split(matter_pair);
	Matter a = getMatterByName(sp.next(','));
	Matter b = getMatterByName(sp.next(','));
	if (a == NULL || b == NULL) {
		throw 114517;
	}
	vector<Reaction> rs = getPossibleReactions(a, b);
	return to_string(rs);
}

// Interfaces

void Init() {
	ReadCMCtable("CMCtable.csv");
}

void GenerateCanReactMatrix() {
	GenerateCanReactMatrix("can_react.matrix");
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