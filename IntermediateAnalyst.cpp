#include "common.h"
#include "IntermediateAnalyst.h"

Quaternion::Quaternion(string op, string arg1, string arg2, string res)
{
	this->op = op;
	this->arg1 = arg1;
	this->arg2 = arg2;
	this->result = res;
}

Status IntermediateCode::emit(Quaternion q)
{
	result.push_back(q);
	return OK;
}

Status IntermediateCode::backpatch(list<int> List, int q)
{
	for (list<int>::iterator itList = List.begin(); itList != List.end(); itList++)
		result[*itList].result = to_string(q);
	return OK;
}

int IntermediateCode::nextquad()
{
	return result.size();
}

Status IntermediateCode::outputCode()
{
	int cnt = 0;
	for (vector<Quaternion>::iterator it = result.begin(); it != result.end(); it++, cnt++)
	{
		cout << cnt << ' ' << '(' << it->op << ',' << it->arg1 << ',' << it->arg2 << ',' << it->result << ')' << endl;
	}
	return OK;
}

Status IntermediateCode::outputCodeToFile()
{
	ofstream out;
	out.open("outputIntermediateCode.txt", ios::out);
	if (!out.is_open())
	{
		cerr << "打开文件outputIntermediateCode.txt失败" << endl;
		exit(-1);
	}
	int cnt = 0;
	for (vector<Quaternion>::iterator it = result.begin(); it != result.end(); it++, cnt++)
	{
		out << cnt << ' ' << '(' << it->op << ',' << it->arg1 << ',' << it->arg2 << ',' << it->result << ')' << endl;
	}
	return OK;
}