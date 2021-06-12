#pragma once

#include "common.h"
#include "IntermediateAnalyst.h"

enum Vtype
{
	Vvoid,
	Vint
};

class Symbol
{
public:
	string content;
	bool Vt;
	string name; // ����ID/NUM��Ч
	int quad;
	list<int> nextlist;
	list<int> falselist;
	list<int> truelist;

	list<string> par;
	list<Vtype> parType;

	Symbol(string content, bool Vt);
	Symbol(string content, bool Vt, string name);
	Symbol();
	// ���������<����������ʵ����map��keyΪclass�Ľṹ
	friend bool operator <(const Symbol& a, const Symbol& b);

	friend bool operator ==(const Symbol& a, const Symbol& b);
};

class SymbolID :public Symbol
{
private:
	string idName;
public:
	SymbolID(Symbol s, string name);
};

class SymbolNum :public Symbol
{
private:
	string numName;
public:
	SymbolNum(Symbol s, string num);
};

class Production
{
public:
	int id;
	Symbol lcontent;
	vector<Symbol> rcontent;
	Production();
};

class Item
{
public:
	int posPoint; // ע�����λ��
	Symbol expect; // չ����
	int idProduction; // ����ʽid
	Item(int posPoint, Symbol expect, int idProduction);
	friend bool operator <(const Item& a, const Item& b);

	friend bool operator ==(const Item& a, const Item& b);

};

class I // ��Ŀ��
{
public:
	set<Item> Items;
};

typedef pair<int, Symbol> GOTO;


class DFA
{
public:
	list<I> status;
	map<GOTO, int> gotos; //��ǰ״̬����GOTO����ǰ̬����symbol���������mapӳ���gotos��int��
};

class Variable
{
public:
	string name;
	Vtype type;
	int depth;
	Variable(string name, Vtype type);
	Variable(string name, Vtype type, int depth);
};

class Function
{
public:
	string name;
	Vtype type;
	list<Vtype> parType;
	int enter; // ������ڣ�����call��ʱ������Ҫ������ڵ�
	Function(string name, Vtype type, list<Vtype> parType);
	Function(string name, Vtype type, list<Vtype> parType, int enter);
};

enum Behave { shift, reduct, accept, error };
class Behavior
{
public:
	Behave bh;
	int nextS;
	Behavior();
	Behavior(Behave bh, int s);

};

class ParAnalyst
{
private:
	ifstream inputProductions;
	// ifstream inputLex;
	vector<Production> Productions;
	map<Symbol, set<Symbol>> First;
	map<Symbol, set<Symbol>> Follow;
	DFA DFA;
	map<GOTO, Behavior> ActionTb;
	map<GOTO, int> GotoTb;
	stack<int> StatusStack;
	stack<Symbol> SymbolStack;
	IntermediateCode code;
	vector<Variable> variableTable; // ������
	vector<Function> functionTable; // ������
	int line; // ����
	int level; // ��ǰ�������ڵĲ㼶
public:
	ParAnalyst();
	ParAnalyst(string path);
	Status openInputProductions(string path);
	Status getProductions();
	Status showProductions();
	Status initFirst();
	// Status initFollow();
	Status showFirst();
	Status createDFA();
	I Closure(Item item);
	set<Symbol> getExpect(list<Symbol> S);
	Status outputAction();
	Status outputActionToFile();
	Status outputGoto();
	Status outputGotoToFile();
	Status LRAnalyse(list<Token> LexRes);
	Status outputStatusStack();
	Status outputSymbolStack();
	Status outputStack(int cnt);
	Status outputStackToFile(int cnt);
	Symbol pop();
	IntermediateCode* getIntermediateCode();
	vector<pair<int, string> > getFunctionEnter();

	// ƥ�亯����ƥ�����
	Function* findFunction(string name);
	Variable* findVariable(string name);
};



