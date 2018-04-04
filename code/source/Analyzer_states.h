#pragma once
#include "lin_Table.h"
#include <map>
#include <utility>
#include <memory>
#include <utility>

using std::pair;
using std::shared_ptr;

class Element_base;
class Element_final;
class Element_simple;
class Element_vector;
class Element_call;

#define tableType vector<shared_ptr<Element_base>>
#define samestates unordered_map<long, vector<int>>
#define states_t std::map<int, shared_ptr<Element_base>>
#define simple_states std::map<int, vector<pair<string, pair<int, vector<int>>>>>

class Analyzer_states
{
private:
	shared_ptr<Element_base> getState(lin_Table & tbl, tableType& table, int pos);
	void simplify(int state, shared_ptr<Element_base> e, vector<int>& st, lin_Table &table);

public:
	states_t States;
	simple_states SS;

	Analyzer_states(lin_Table & tbl);
	void printStates(std::ostream & out, lin_Table &t);

	simple_states getStates(lin_Table & table);

	~Analyzer_states();
};

