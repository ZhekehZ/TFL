#pragma once

#include "Analyzer_states.h"

class Analyzer
{
private:
public:
	simple_states states;

	int Analyzer::rec_analyzer(const string & str, int l, int state, vector<int> & st);

public:

	int analyze(const string & str);

	Analyzer(simple_states& ss) : states(ss) {};
	Analyzer(istream & input);
};

