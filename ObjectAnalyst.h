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
	vector<string> objectCode; // ������Ŀ�����
	map<string, int> variableStorge;

	map<string, set<string> > RVALUE;
	map<string, set<string> > AVALUE;
	list<string> registerList;

	// ��ǰ�����ĺ���������Ϣ�Ļ����飬����Ϣ����Ԫʽ
	string functionAnalysing;
	vector<BaseBlockWithInfo>::iterator baseBlockWithInfoAnalysing;
	vector<QuaternionAttachInfo>::iterator quaternionAttachInfoAnalysing;

	// ��ǰջ��
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