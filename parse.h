#pragma once

#include"atom.h"
#include<string>

class Split {
public:
	const char* ptr;
	const char* ptrnxt;
	char* buffer;

	Split(string& str) {
		ptr = str.c_str();
		ptrnxt = ptr;
		buffer = new char[str.length()];
	}
	string next(char delimiter) {
		while (*ptrnxt != '\0' && *ptrnxt++ != delimiter) {
			;
		}
		int len = *ptrnxt == '\0' ? ptrnxt - ptr : ptrnxt - ptr - 1;
		memcpy(buffer, ptr, len * sizeof(char));
		buffer[len] = '\0';
		ptr = ptrnxt;
		return string(buffer);
	}
	bool goes() {
		return *ptr != '\0';
	}
};

enum class CharType {
	Uppercase, Lowercase, Number, Lbracket, Rbracket, Illegal
};
enum class ParserState {
	Void, Element, Number
};
enum class SymbolType {
	Formula, Lbracket, Number
};

#define ILLEGAL_NAME_CASE 114514
#define ILLEGAL_ELEMENT_CASE 114515

inline CharType getCharType(char c) {
	if (c >= 'A' && c <= 'Z') {
		return CharType::Uppercase;
	}
	else if (c >= 'a' && c <= 'z') {
		return CharType::Lowercase;
	}
	else if (c >= '0' && c <= '9') {
		return CharType::Number;
	}
	else if (c == '(') {
		return CharType::Lbracket;
	}
	else if (c == ')') {
		return CharType::Rbracket;
	}
	else {
		return CharType::Illegal;
	}
}

inline int stringToId(string& str) {
	const string* p = Atom_list;
	const string* pend = Atom_list + Element_num;
	for (; p < pend; p++) {
		if (*p == str) {
			return p - Atom_list;
		}
	}
	throw ILLEGAL_ELEMENT_CASE;
	return -1;
}

typedef struct FormulaSymbolUnion {
	SymbolType symbol;
	Formula formula;
} FSU;

inline FormulaSymbolUnion stringToFSU(string& str) {
	return { SymbolType::Formula, atom(stringToId(str)) };
}
inline FormulaSymbolUnion LbracketFSU() {
	return { SymbolType::Lbracket, zero() };
}

inline void clear_buffer_push_str(char*& bufptr, char* buffer, vector<FSU>& words) {
	*bufptr = '\0';
	string str = string(buffer);
	if (!str.empty()) {
		if (str[0] >= '0' &&  str[0] <= '9') {
			words.back().formula *= stoi(str);	// multiply, do not push back
		}
		else {
			words.push_back(stringToFSU(str));
		}
	}
	bufptr = buffer;
}

Formula formulaFromName(const string& _name) {
	string name = _name + "X";				// "X" itself will not be parsed, but it refreshes the state
	ParserState state = ParserState::Void;
	char buffer[8];
	char* bufptr = buffer;
	vector<FSU> words;
	for (const char& ch : name) {
		CharType ct = getCharType(ch);
		switch (ct)
		{
		case CharType::Uppercase:
			clear_buffer_push_str(bufptr, buffer, words);
			state = ParserState::Element;
			*bufptr++ = ch;
			break;
		case CharType::Lowercase:
			*bufptr++ = ch;
			break;
		case CharType::Number:
			if (state == ParserState::Number) {
				;
			}
			else{
				clear_buffer_push_str(bufptr, buffer, words);
				state = ParserState::Number;
			}
			*bufptr++ = ch;
			break;
		case CharType::Lbracket:
			clear_buffer_push_str(bufptr, buffer, words);
			words.push_back(LbracketFSU()); 
			state = ParserState::Void;
			break;
		case CharType::Rbracket:
			{
			clear_buffer_push_str(bufptr, buffer, words);
				FSU a = words.back(); words.pop_back();
				while (words.back().symbol != SymbolType::Lbracket) {
					a.formula += words.back().formula;
					words.pop_back();
				}
				words.pop_back();		// pop out the left bracket symbol
				words.push_back(a);		// push back the result
			}
			break;
		case CharType::Illegal:
		default:
			throw ILLEGAL_NAME_CASE;
			break;
		}
	}
	Formula res = zero();
	for (auto fsu : words) {
		res += fsu.formula;
	}
	return res;
}