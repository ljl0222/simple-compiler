#pragma once
#include "common.h"

class Quaternion
{
public:
	string op;
	string arg1;
	string arg2;
	string result;
	Quaternion();
	Quaternion(string, string, string, string);
};

class BaseBlock
{
public:
	string name;
	vector<Quaternion> qCodes;
	pair<int, int> ret;
};

class IntermediateCode
{
private:
	vector<Quaternion> result;
	map<string, vector<BaseBlock> > functionBlocks;
public:
	int T;
	Status emit(Quaternion q);
	Status backpatch(list<int> List, int q);
	int nextquad();
	Status outputCode();
	Status outputCodeToFile();
	Status divideBaseBlocks(vector<pair<int, string>> functionEnter);
	Status outputBlocks();
	map<string, vector<BaseBlock> > *getFunctionBlocks();
};