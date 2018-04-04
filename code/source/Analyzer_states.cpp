#include "Analyzer_states.h"

class Element_base {
public:
	Element_base() {}
	virtual int getType() = 0;
	virtual void print(std::ostream & out, lin_Table & tbl) = 0;
	virtual long hash(tableType * t) = 0;
	virtual ~Element_base() {};
	virtual void update(samestates & m, tableType & t, bool * valid) = 0;
};

class Element_final : public Element_base {
public:
	Element_final() {};
	int getType() {
		return 0;
	}
	void print(std::ostream & out, lin_Table & tbl) {
		out << " { F } ";
	};
	long hash(tableType * t) {
		return 1;
	}
	~Element_final() {};
	void update(samestates & m, tableType & t, bool * valid) {}
};

class Element_simple : public Element_base {
public:
	int term, pos;
	long H;
	Element_simple(int t, int p) : H(0), term(t), pos(p) {};
	int getType() {
		return 1;
	}
	void print(std::ostream & out, lin_Table & tbl) {
		out << " { " << tbl.getFCTstr(term) << "' - " << pos << " } ";
	};
	long hash(tableType * t) {
		if (H != 0)
			return H;
		//H = (*t)[pos + 1]->hash(t) + term * 1000; ///stack overflow
		H = pos + term * 1000;
		return H;
	}
	~Element_simple() {};
	void update(samestates & m, tableType & t, bool * valid) {
		pos = m.at(t[pos + 1]->hash(nullptr))[0];
		valid[pos] = true;
	}
};

class Element_vector : public Element_base {
public:
	long H;
	vector<shared_ptr<Element_base>> elements;
	Element_vector(shared_ptr<Element_base> e1, shared_ptr<Element_base> e2) : H(0){
		if (e1->getType() == 2) {
			Element_vector * E = static_cast<Element_vector*>(e1.get());
			for (auto a : E->elements)
				elements.push_back(a);
		}
		else
			elements.push_back(e1);
		if (e2->getType() == 2) {
			Element_vector * E = static_cast<Element_vector*>(e2.get());
			for (auto a : E->elements)
				elements.push_back(a);
		}
		else
			elements.push_back(e2);
	};
	int getType() {
		return 2;
	}
	~Element_vector(){}
	void print(std::ostream & out, lin_Table & tbl) {
		out << " { ";
		for (int i = 0; i < elements.size(); i++) {
			if (i != 0)
				out << " , ";
			elements[i]->print(out, tbl);
		}
		out << " } ";
	};
	long hash(tableType * t) {
		if (H != 0)
			return H;
		long sum = 0;
		for (auto a : elements) 
			sum += a->hash(t);
		H = sum;
		return H;
	}
	void update(samestates & m, tableType & t, bool * valid) {
		for (auto a : elements)
			a->update(m, t, valid);
	}
};

class Element_call : public Element_base {
public:
	int Nont, retpos;
	shared_ptr<Element_base> element;
	long H;
	Element_call(int n, int p, shared_ptr<Element_base> e) : H(0), Nont(n), retpos(p), element(e) {};
	int getType() {
		return 3;
	}
	~Element_call() {}
	void print(std::ostream & out, lin_Table & tbl) {
		out << " ( ";
		element->print(out, tbl);
		out << " , " << tbl.getFCTstr(Nont) << " - " << retpos << " ) ";
	}
	long hash(tableType * t) {
		if (H != 0)
			return H;
		H = (*t)[retpos]->hash(t) * 10000 + retpos + element->hash(t);
		return H;
	}
	void update(samestates & m, tableType & t, bool * valid) {
		if (m.find(t[retpos + 1]->hash(nullptr)) != m.end())
			retpos = m[t[retpos + 1]->hash(nullptr)][0];
		element->update(m, t, valid);
		valid[retpos] = true;
	};
};

shared_ptr<Element_base> Analyzer_states::getState(lin_Table & tbl, tableType& table, int pos = -1) {
	if (table.size() > pos + 1) {
		return table.at(pos + 1);
	}
	int next = pos + 1;
	if (next >= tbl.table.size()) {
		return shared_ptr<Element_base>(new Element_final());
	}
	if (tbl.table[next].t == lin_Table::element::NUM) {
		return getState(tbl, table, tbl.table[next].val - 1);
	}
	else if (tbl.table[next].t == lin_Table::element::NONT) {
		return getState(tbl, table, tbl.table[next].val - 1);
	}
	else if (tbl.table[next].t == lin_Table::element::TERM) {
		return shared_ptr<Element_base>(new Element_simple(tbl.table[next].val, next));
	}
	else if (tbl.table[next].t == lin_Table::element::DOT) {
		return shared_ptr<Element_base>(new Element_final());
	}
	else if (tbl.table[next].t == lin_Table::element::CASES) {
		return shared_ptr<Element_base>(new Element_vector(getState(tbl, table, next), getState(tbl, table, next + 1)));
	}
	else if (tbl.table[next].t == lin_Table::element::ASTER) {
		return shared_ptr<Element_base>(new Element_call(tbl.getNont(tbl.table[next + 1].val), next + 1, getState(tbl, table, next)));
	}
	else if (tbl.table[next].t == lin_Table::element::ARROW) {
		return getState(tbl, table, next);
	}
}

Analyzer_states::Analyzer_states(lin_Table & tbl){
	tableType table;
	samestates same;

	for (int i = 0; i < tbl.table.size(); i++) {
		shared_ptr<Element_base> e = getState(tbl, table, i - 1);
		table.push_back(e);
	}
	same[table[0]->hash(&table)].push_back(-1);
	for (int i = table.size() - 1; i > 0; i--) {
		same[table[i]->hash(&table)].push_back(i - 1);
	}

	bool * valid = new bool[tbl.table.size()];
	std::fill(valid, valid + tbl.table.size(), false);

	for (int i = 0; i < tbl.table.size(); i++) 
		if (same[table[i]->hash(nullptr)][0] == i - 1) {
			table[i]->update(same, table, valid);
		}

	for (int i = 0; i < tbl.table.size(); i++)
		if (same[table[i]->hash(nullptr)][0] == i - 1) {
			if(i == 0 || valid[i-1])
				States[i - 1] = table[i];
		}
	delete[] valid;
};

Analyzer_states::~Analyzer_states() {

}

void Analyzer_states::printStates(std::ostream & out, lin_Table &t) {
	for (auto a : States) {
		if (a.first < 0)
			out << "S_first = ";
		else
			out << "S_" << a.first << " = ";
		a.second->print(out, t);
		out << '\n';
	}
};

void Analyzer_states::simplify(int state, shared_ptr<Element_base> e, vector<int>& st, lin_Table &table) {
	if (e->getType() == 0) {
		vector<int> v(st);
		SS[state].push_back({ "",{ -10, v } });
	}
	else if (e->getType() == 1) {
		vector<int> v(st);
		auto A = static_cast<Element_simple*>(e.get());
		string s = table.getFCTstr(A->term);
		SS[state].push_back({ s, { A->pos, v } });
	}
	else if (e->getType() == 2) {
		auto A = static_cast<Element_vector*>(e.get());
		for (auto b : A->elements)
			simplify(state, b, st, table);
	}
	else if (e->getType() == 3) {
		auto A = static_cast<Element_call*>(e.get());
		st.push_back(A->retpos);
		simplify(state, A->element, st, table);
		st.pop_back();
	}
};

simple_states Analyzer_states::getStates(lin_Table & table) {
	for (auto a : States) {
		vector<int> st;
		simplify(a.first, a.second, st, table);
	}
	return SS;
}