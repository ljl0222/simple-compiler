#pragma once
#include "common.h"

class Quaternion
{
public:
	string op;
	string arg1;
	string arg2;
	string result;
	Quaternion(string, string, string, string);
};

class IntermediateCode
{
private:
	vector<Quaternion> result;
public:
	int T;
	Status emit(Quaternion q);
	Status backpatch(list<int> List, int q);
	int nextquad();
	Status outputCode();
	Status outputCodeToFile();

};