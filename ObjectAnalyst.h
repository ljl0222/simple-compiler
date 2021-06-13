#pragma once

#include "IntermediateAnalyst.h"
#include "common.h"

const int REGNUM = 8;

class StandBy_ActiveInfo
{
public:
	int num;
	bool isActive;

	StandBy_ActiveInfo();
	StandBy_ActiveInfo(int num, bool isActive);
	Status outputStandBy_ActiveInfo();
};

class QuaternionAttachInfo
{
public:
	Quaternion q;
	StandBy_ActiveInfo lres;
	StandBy_ActiveInfo lop;
	StandBy_ActiveInfo rop;

	QuaternionAttachInfo(Quaternion q, StandBy_ActiveInfo lres, StandBy_ActiveInfo lop, StandBy_ActiveInfo rop);
};

class BaseBlockWithInfo
{
public:
	string name;
	vector<QuaternionAttachInfo> qCodes;
	pair<int, int> ret;
};

class ObjectAnalyst
{
private:
	map<string, vector<set<string> > > blockInput;
	map<string, vector<set<string> > > blockOutput;

	map<string, vector<BaseBlockWithInfo> > function_baseBlockWithInfo;
	vector<string> objectCode; // 产生的目标代码
	map<string, int> variableStorge;

	map<string, set<string> > RVALUE;
	map<string, set<string> > AVALUE;
	list<string> registerList;

	// 当前分析的函数，带信息的基本块，带信息的四元式
	string functionAnalysing;
	vector<BaseBlockWithInfo>::iterator baseBlockWithInfoAnalysing;
	vector<QuaternionAttachInfo>::iterator quaternionAttachInfoAnalysing;

	// 当前栈顶
	int stackTop;

public:
	Status analyse(map<string, vector<BaseBlock> >* functionBlocks);
	Status outputBlocksAttachInfo();
	Status productObjectCode_total();
	Status productObjectCode_baseBlockWithInfo(map<string, vector<BaseBlockWithInfo> >::iterator &i);
	Status productObjectCode_baseBlock(int index);
	Status productObjectCode_quaternion(int index, int &argNum, int &parNum, list<pair<string, bool> > &parList);
	Status storeAllVariable(set<string> &output);
	Status storeVariable(string reg, string variable);

	string allocateRegister(string variable);
	string allocateRegister();
	Status freeVariable(string variable);
	string disRegister();

	Status outputObjectCode();
};