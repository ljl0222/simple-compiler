#pragma once

#include "IntermediateAnalyst.h"
#include "common.h"

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
public:
	Status analyse(map<string, vector<BaseBlock> >* functionBlocks);
	Status outputBlocksAttachInfo();
};