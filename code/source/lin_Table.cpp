#include "lin_Table.h"

void lin_Table::recgen(int i, int l, int r) {
	if (l == r) return;
	static int a0 = encoder.getTint("(");
	static int a1 = encoder.getTint(")");
	static int a2 = encoder.getTint(",");
	static int a3 = encoder.getTint(";");
	static int a4 = encoder.getTint("#");
	static int a5 = encoder.getTint("[");
	static int a6 = encoder.getTint("]");
	static int a7 = encoder.getTint(".");
	static int a8 = encoder.getTint("*");

	int counter = 0;
	int ta2 = -1, ta3 = -1, ta4 = -1, ta8 = -1;
	for (int j = l; j < r; j++) {
		if (encoder.encoded[i][j] == a0 || encoder.encoded[i][j] == a5)
			counter++;
		if (encoder.encoded[i][j] == a1 || encoder.encoded[i][j] == a6)
			counter--;
		if (encoder.encoded[i][j] == a2 && counter == 0)
			ta2 = j;
		if (encoder.encoded[i][j] == a3 && counter == 0)
			ta3 = j;
		if (encoder.encoded[i][j] == a4 && counter == 0)
			ta4 = j;
		if (encoder.encoded[i][j] == a8 && counter == 0)
			ta8 = j;
	}
	if (ta3 != -1) {
		int sizeL = ta3 - l, sizeR = r - ta3 - 1;
		if (sizeL != 0 && sizeR != 0) {
			element c{element::CASES, 0};
			element v{ element::NUM, 0 };
			table.push_back(c);
			table.push_back(v);
			int pos = table.size() - 1;
			recgen(i, l, ta3);
			element a{ element::ARROW, 0 };
			element w{ element::NUM, 0 };
			table.push_back(a);
			table.push_back(w);
			table[pos].val = table.size();
			pos = table.size() - 1;
			recgen(i, ta3 + 1, r);
			table[pos].val = table.size();
		}
		else {
			if (sizeL == 0) {
				l = ta3 + 1;
				ta3 = r;
			}
			element c{ element::CASES, 0 };
			element v{ element::NUM, 0 };
			table.push_back(c);
			table.push_back(v);
			int pos = table.size() - 1;
			recgen(i, l, ta3);
			table[pos].val = table.size();
		}
	}
	else if(ta2 != -1){
		recgen(i, l, ta2);
		recgen(i, ta2 + 1, r);
	}
	else if (ta4 != -1) {//A# == A<ae
		int sizeL = ta4 - l, sizeR = r - ta4 - 1;
		if (sizeR != 0) {
			int posA = table.size();
			recgen(i, l, ta4);
			element c{ element::CASES, 0 };
			element v{ element::NUM, 0 };
			table.push_back(c);
			table.push_back(v);
			int pos = table.size() - 1;
			recgen(i, ta4 + 1, r);
			element a{ element::ARROW, 0 };
			element w{ element::NUM, posA };
			table.push_back(a);
			table.push_back(w);
			table[pos].val = table.size();
		}
		else {
			int posA = table.size();
			recgen(i, l, ta4);
			element c{ element::CASES, 0 };
			element v{ element::NUM, posA };
			table.push_back(c);
			table.push_back(v);
		}
	}
	else if (ta8 != -1) {
		int posA = table.size();
		element c{ element::CASES, 0 };
		element v{ element::NUM, 0 };
		table.push_back(c);
		table.push_back(v);
		int pos = table.size() - 1;
		recgen(i, l, ta8);
		element a{ element::ARROW, 0 };
		element w{ element::NUM, posA };
		table.push_back(a);
		table.push_back(w);
		table[pos].val = table.size();
	}
	else if (encoder.encoded[i][l] == a0) {
		recgen(i, l + 1, r - 1);
	}
	else if (encoder.encoded[i][l] == a5) {
		element c{ element::CASES, 0 };
		element v{ element::NUM,0 };
		table.push_back(c);
		table.push_back(v);
		int pos = table.size() - 1;
		recgen(i, l + 1, r - 1);
		table[pos].val = table.size();
	}
	else if (r - l == 1) {
		int a = encoder.encoded[i][l];
		if (encoder.getType(a) == c_Table::TERM) {
			table.push_back({ element::TERM, a });
		}
		else if (encoder.getType(a) == c_Table::NONT) {
			table.push_back({ element::ASTER, 0 });
			table.push_back({ element::NONT, a });
		}
	}
	else
		throw "L_TABLE GEN ERROR";
}

void lin_Table::generate(int i) {
	int n = table.size();
	nonts[encoder.encoded[i][0]] = n;
	recgen(i, 2, encoder.encoded[i].size() - 1);
	table.push_back({ element::DOT, 0 });
}

lin_Table::lin_Table(istream & input) {
	encoder.Encode(input);
	for (int i = 0; i < encoder.encoded.size(); i++) {
			generate(i);
	}
	for (int i = 0; i < table.size(); i++) {
		if (table[i].t == element::NONT)
			table[i].val = nonts[table[i].val];
	}
}

string lin_Table::elem_to_str(lin_Table::element e) {
	std::stringstream ss;
	switch (e.t)
	{
	case lin_Table::element::NUM:
		ss << e.val;
		return ss.str();
		break;
	case lin_Table::element::TERM:
		return encoder.getTstr(e.val).substr(1);
		break;
	case lin_Table::element::NONT:
		ss << e.val;
		return ss.str();
		break;
	case lin_Table::element::ARROW:
		return string(1, char(25));
		//return "↓";
		break;
	case lin_Table::element::ASTER:
		return "*";
		break;
	case lin_Table::element::CASES:
		return "<";
		break;
	case lin_Table::element::DOT:
		return ".";
		break;
	default:
		break;
	}
}

#include <fstream>
void lin_Table::print(std::ostream & out) {
	//std::ofstream str("t_out");
	int S = 11;
	
	char vert = char(186);
	char luc = char(201);
	char hor = char(205);
	char ruc = char(187);
	char upc = char(203);
	char cro = char(206);
	char lco = char(204);
	char dco = char(202);
	char rco = char(185);
	char rdc = char(188);
	char ldc = char(200);
	
	out << luc << std::setfill(hor);
	for (int i = 0; i < 10; i++) out << std::setw(S + 1) << upc;
	out << std::setw(S + 1) << ruc << '\n' << std::right << std::setfill(' ');

	//str << "\\hline\n";

	out << vert << std::setw(S + 1) << vert << std::left;
	for (int i = 0; i < 10; i++) out << std::setw(S) << i << vert;
	out << '\n' << std::right;

	//str << "";
	//for (int i = 0; i < 10; i++) str << " & " << i;
	//str << "\\\\\n\\hline\n";

	for (int i = 0; i <= table.size() / 10; i++) {

		out << lco << std::setfill(hor);
		for (int i = 0; i < 10; i++) out << std::setw(S + 1) << cro;
		out << std::setw(S + 1) << rco << '\n' << std::right << std::setfill(' ');

		out << vert << std::left << std::setw(S) << i << std::right;

		//str << i << "&";
		for (int j = 0; j < 10; j++) {
			if (i * 10 + j < table.size()) {
				out << vert << std::setw(S) << elem_to_str(table[i * 10 + j]);
				//str << elem_to_str(table[i * 10 + j]) << " & ";
			}
			else {
				//str<< "\\ " << "&" ;
				out << std::left << std::setw(S + 1) << vert << std::right;
			}
		}
		//str << "\\\\\n\\hline\n";
		out << vert << '\n';
	}

	out << ldc << std::setfill(hor);
	for (int i = 0; i < 10; i++) out << std::setw(S + 1) << dco;
	out << std::setw(S + 1) << rdc << '\n' << std::right << std::setfill(' ');
}

int lin_Table::getNont(int pos) {
	for (auto a : nonts) {
		if (a.second == pos)
			return a.first;
	}
	return -1;
};

string lin_Table::getFCTstr(int i) {
	return encoder.getTstr(i);
};