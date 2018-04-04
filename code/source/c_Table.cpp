#include "c_Table.h"

constexpr int MAXN = 1000;

void c_Table::initDefault() {
	vector<string> t_nonterms = { ":", "(", ")", ".", "*", ";", ",", "#", "[", "]", "Eofgram" };
	nonterms = t_nonterms;
}

int c_Table::get_int(const string & s) {
	for (auto a = nonterms.begin(); a != nonterms.end(); a++) {
		if (*a == s) {
			return int(a - nonterms.begin());
		}
	}
	for (auto a = terms.begin(); a != terms.end(); a++) {
		if (*a == s) {
			return int(a - terms.begin()) + MAXN;
		}
	}
	///throw ("ERROR - UNKNOWN TABLE ELEMENT [string]");
	return -1;
}

string c_Table::get_string(int i) {
	if (i < MAXN)
		return nonterms[i];
	i -= MAXN;
	if (i < MAXN)
		return terms[i];
}

c_Table::c_Table()
{
	initDefault();
}

void c_Table::optimize() {
	nonterms.shrink_to_fit();
	terms.shrink_to_fit();
}

int c_Table::add(const string & s, c_Table::ct_TYPE type) {
	if (get_int(s) != -1)
		return get_int(s);
	switch (type){
	case c_Table::NONT:
		nonterms.push_back(s);
		break; 
	case c_Table::TERM:
		terms.push_back(s);
		break; 
	}
	return get_int(s);
};

c_Table::ct_TYPE c_Table::getType(int i) {
	if (i < MAXN)
		return NONT;
	i -= MAXN;
	if (i < MAXN)
		return TERM;
	throw "NO TYPE";
}