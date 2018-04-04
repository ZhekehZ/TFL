#include "Analyzer.h"

bool is_pref(const string & str1, const string & str2, int off) {
	string str = str1.substr(1);
	size_t res = str2.find(str, off);
	if (res == string::npos)
		return false;
	if (res > off)
		return false;
	return true;
}

#include <iostream>
//todo stack & other
int Analyzer::rec_analyzer(const string & str, int l, int state, vector<int> & st) {
	if (state == -10) std::cout << -10;
	if (l >= str.size())
		return -1;
	int L = l;
	for (auto a : states[state]) {
		if (a.second.first == -10) {
			vector<int> temp(st);
			if (temp.size() > 0) {
				state = temp.back();
				temp.pop_back();
				int L1 = rec_analyzer(str, l, state, temp);
				if (L1 > L) L = L1;
			}
			else {
				if (l >= str.length() - 1) {
					return INT_MAX;
				}
				else
					return l;
			}
		}
		else {
			if (is_pref(a.first, str, l)) {
				vector<int> temp(st);
				for (auto t : a.second.second)
					temp.push_back(t);
				int L1 = rec_analyzer(str, l + a.first.length() - 1, a.second.first, temp);
				if (L1 > L) L = L1;
			}
		}
	}
	return L;
}

int Analyzer::analyze(const string & str) {
	return rec_analyzer(str, 0, -1, vector<int>());
}

Analyzer::Analyzer(istream & input) {
	lin_Table table(input);
	Analyzer_states states_(table);
	states = states_.getStates(table);
}