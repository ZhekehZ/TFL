#include <iostream>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include "Analyzer.h"

using namespace std;

HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);

string delspaces(const string & s) {
	string res;
	for (auto c : s) {
		if (c != ' ' && c != '\t' && c != '\n')
			res.push_back(c);
	}
	res.push_back(' ');
	return res;
}

void printcolor(int n, const string & s) {
	int m = n;
	int i = 0;
	for (; (i < s.length()) && (m != 0); i++) {
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
			m--;
	}
	SetConsoleTextAttribute(H, 10);
	cout << s.substr(0, i);
	SetConsoleTextAttribute(H, 12);
	cout << s.substr(i);
	
	if (n != INT_MAX && i == s.length()) {
		SetConsoleTextAttribute(H, 76);
		cout << " ? ";
	}
	cout << '\n';
	SetConsoleTextAttribute(H, 7);
}

int main() {
	CONSOLE_FONT_INFOEX cfie;
	cfie.cbSize = sizeof(cfie);
	cfie.nFont = 0;
	cfie.dwFontSize = { 18, 30 };
	cfie.FontFamily = FF_DONTCARE;
	cfie.FontWeight = FW_NORMAL;
	wcscpy_s(cfie.FaceName, L"Terminal");
	SetCurrentConsoleFontEx(H, false, &cfie);

	cout << "input grammar file name\n>";
	string s;
	cin >> s;
	system("cls");

	cout << "[1] - input from keyboard\n[0] - input from file\n\n[2] - print table & states to file\n(type 0 / 1 / 2)\n";
	char t = _getwch();
	if ( t == '1') {
		ifstream istr(s);
		Analyzer A(istr);
		istr.close();
		system("cls");
		cout << "(keyboard mode | input something | print \"end.\" to exit)\n>";
		cin.get();
		do {
			getline(cin, s);
			if (s != "end.") {
				int res = A.analyze(delspaces(s));
				if (res < 0) res = 0;
				system("cls");
				printcolor(res, s);
				cout << "\nResult: ";
				if (res == INT_MAX)
					cout << "correct input";
				else
					cout << "error at pos " << res + 1;
				cout << "\n>";
			}
		} while (s != "end.");
	}
	else if (t == '0')
	{
		ifstream istr(s);
		Analyzer A(istr);
		istr.close();
		system("cls");
		cout << "(file mode | input file name | print \"end.\" to exit)\n>";
		cin.get();
		while (getline(cin, s)) {
			if (s == "end.")
				break;
			ifstream input(s);
			string test;
			while (getline(input, s))
				test += "\n" + s;
			input.close();
			int res = A.analyze(delspaces(test));
			if (res < 0) res = 0;
			system("cls");
			printcolor(res, test);
			cout << "\nResult: ";
			if (res == INT_MAX)
				cout << "correct input";
			else
				cout << "error at pos " << res + 1;
			cout << "\n>";
		}
	}
	else{
		system("cls");
		ifstream ifs(s);
		lin_Table lt(ifs);
		Analyzer_states states_(lt);
		cout << "[table] input file name\n>";
		cin >> s;
		lt.print(ofstream(s));
		cout << "[states] input file name\n>";
		cin >> s;
		states_.printStates(ofstream(s), lt);
		cin.get();
	}
	
	
	return 0;
}