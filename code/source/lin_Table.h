#pragma once
#include "c_Encoder.h"
#include <unordered_map>
#include <iomanip>
#include <ostream>
#include <sstream>

using std::unordered_map;

class lin_Table
{
private:
	public:
	class element;

	c_Encoder encoder;

	void recgen(int i, int l, int r);
	void generate(int i);
	string elem_to_str(element e);
	unordered_map<int, int> nonts;

public:
	class element {
	public:
		enum elType {
			NUM = 0,
			TERM = 1,
			NONT = 2,
			ARROW = 3,
			ASTER = 4,
			CASES = 5,
			DOT = 6
		};

		elType t;
		int val;
	};

	vector<element> table;
	int getNont(int pos);
	
	string getFCTstr(int i);
	
	lin_Table(istream & input);
	void print(std::ostream & out);
};

