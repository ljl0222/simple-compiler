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
	string name; // 仅当ID/NUM有效
	int quad;
	list<int> nextlist;
	list<int> falselist;
	list<int> truelist;

	list<string> par;
	list<Vtype> parType;

	Symbol(string content, bool Vt);
	Symbol(string content, bool Vt, string name);
	Symbol();
	// 重载运算符<，这样才能实例化map中key为class的结构
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
	int posPoint; // 注明点的位置
	Symbol expect; // 展望符
	int idProduction; // 产生式id
	Item(int posPoint, Symbol expect, int idProduction);
	friend bool operator <(const Item& a, const Item& b);

	friend bool operator ==(const Item& a, const Item& b);

};

class I // 项目集
{
public:
	set<Item> Items;
};

typedef pair<int, Symbol> GOTO;


class DFA
{
public:
	list<I> status;
	map<GOTO, int> gotos; //当前状态读到GOTO（当前态读到symbol到哪里），到map映射的gotos的int处
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
	int enter; // 函数入口，在我call的时候是需要函数入口的
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
	vector<Variable> variableTable; // 变量表
	vector<Function> functionTable; // 函数表
	int line; // 行数
	int level; // 当前变量处于的层级
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

	// 匹配函数，匹配变量
	Function* findFunction(string name);
	Variable* findVariable(string name);
};



