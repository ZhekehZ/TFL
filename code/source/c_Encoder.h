#pragma once
#include "c_Table.h"
#include <istream>

using std::istream;

class c_Encoder
{
private: 
	string getNxt(istream & input);
	string getNword(const string & s, int & n);
	vector<int> enc(const string & s);

	c_Table table;
public:
	vector<vector<int>> encoded;
	
	int getTint(const string & s);
	string getTstr(int i);
	c_Table::ct_TYPE getType(int i);

	void Encode(istream & input);
};

