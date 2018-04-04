#include "c_Encoder.h"

string c_Encoder::getNxt(istream & input) {
	bool free = true;
	string next = "";
	char c;
	input >> c;
	while (true)
	{
		if (c == '\'')
			free = !free;
		if((c != ' ' && c != '\t' && c != '@') || !free)
			next.push_back(c);
		if (free && c == '.')
			break;
		input >> c;
	}
	next.shrink_to_fit();
	return next;
}

string c_Encoder::getNword(const string & s, int & n) {
	if (s[n] == ' ') {
		n++;
	}
	int m = n;
	if (s[m] == '\'') {
		int count = s.find_first_of('\'', m + 1) - m;
		n = m + count + 1;
		return s.substr(m, count);
	}
	if (isalpha(s[m])) {
		int count;
		if (s.find_first_of(":().*;,#[]'", m) == string::npos)
			count = s.size() - m;
		else
			count = s.find_first_of(":().*;,#[]'", m) - m;
		n = m + count;
		return s.substr(m, count);
	}
	if (s[m] == ':' || s[m] == '(' || s[m] == ')' || s[m] == '.' || s[m] == '*' ||
		s[m] == ';' || s[m] == ',' || s[m] == '#' || s[m] == '[' || s[m] == ']'){
		n++;
		return string(1, s[m]);
	}
	throw ("READ WORD ERROR");
}

vector<int> c_Encoder::enc(const string & s) {
	int n = 0;
	string temp;
	vector<int> encoded_line;
	while (n != s.size()) {
		temp = getNword(s, n);
		c_Table::ct_TYPE type;
		if (temp[0] == '\'') 
			type = c_Table::TERM;
		else
			type = c_Table::NONT;

		encoded_line.push_back(table.add(temp, type));
	}
	encoded_line.shrink_to_fit();
	return encoded_line;
}

void c_Encoder::Encode(istream & input) {
	encoded.clear();
	string s = getNxt(input);
	while (s != "Eogram.") {
		encoded.push_back(enc(s));
		s = getNxt(input);
	}
	encoded.shrink_to_fit();
};

int c_Encoder::getTint(const string & s) {
	return table.get_int(s);
};

string c_Encoder::getTstr(int i) {
	return table.get_string(i);
};

c_Table::ct_TYPE c_Encoder::getType(int i) {
	return table.getType(i);
};