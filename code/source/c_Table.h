#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;

class c_Table
{
private:
	vector<string> nonterms;
	vector<string> terms;

	void initDefault();
public:
	enum ct_TYPE {
		NONT = 0,
		TERM = 1
	};

	c_Table();

	int get_int(const string & s);
	string get_string(int i);

	int add(const string & s, ct_TYPE type);
	ct_TYPE getType(int i);

	void optimize();
};

