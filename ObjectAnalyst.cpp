#include "common.h"
#include "ObjectAnalyst.h"

StandBy_ActiveInfo::StandBy_ActiveInfo()
{

}

StandBy_ActiveInfo::StandBy_ActiveInfo(int num, bool isActive)
{
	this->num = num;
	this->isActive = isActive;
}

QuaternionAttachInfo::QuaternionAttachInfo(Quaternion q, StandBy_ActiveInfo lres, StandBy_ActiveInfo lop, StandBy_ActiveInfo rop)
{
	this->q = q;
	this->lres = lres;
	this->lop = lop;
	this->rop = rop;
}

Status ObjectAnalyst::analyse(map<string, vector<BaseBlock> >* functionBlocks)
{
	for (map<string, vector<BaseBlock> >::iterator i = functionBlocks->begin(); i != functionBlocks->end(); i++) {
		vector<BaseBlockWithInfo> baseBlocksWithInfo;
		vector<BaseBlock> &baseBlocks = i->second;

		// 已经被定义，已经使用过
		vector<set<string> > isDefine, isUsed;

		// 输入和输出
		vector<set<string> > input, output;

		// 遍历每一个基本块当中的每一个四元式
		for (vector<BaseBlock>::iterator j = baseBlocks.begin(); j != baseBlocks.end(); j++) {
			set<string> isdef, isuse;
			for (vector<Quaternion>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				if (k->op == "j==" || k->op == "j!=" || k->op == "j<=" || k->op == "j<" || k->op == "j>=" || k->op == "j>") {
					if (isalpha(k->arg1[0]) && isdef.count(k->arg1) == 0) {
						isuse.insert(k->arg1); // 已经被使用了，但是没有定值
					}
					if (isalpha(k->arg2[0]) && isdef.count(k->arg2) == 0) {
						isuse.insert(k->arg2); // 已经被使用了，但是没有定值
					}
				}
				else if (k->op != "j" && k->op != "call") { // 不考虑无条件语句
					if (isalpha(k->arg1[0]) && isdef.count(k->arg1) == 0) {
						isuse.insert(k->arg1); // 已经被使用了，但是没有定值
					}
					if (isalpha(k->arg2[0]) && isdef.count(k->arg2) == 0) {
						isuse.insert(k->arg2); // 已经被使用了，但是没有定值
					}
					if (isalpha(k->result[0]) && isdef.count(k->result) == 0) {
						isuse.insert(k->result); // 已经被使用了，但是没有定值
					}
				}
			}
			input.push_back(isuse);
			isDefine.push_back(isdef);
			isUsed.push_back(isuse);
			output.push_back(set<string>()); // 加入一个空集
		}

		// 确定出入口活跃变量

		bool isChange = true;
		while (1) {
			if (!isChange) { // 如果两个集合不再增加，就退出
				break;
			}

			isChange = false;
			int cnt = 0;
			for (vector<BaseBlock>::iterator i = baseBlocks.begin(); i != baseBlocks.end(); i++, cnt++) {
				if (i->ret.first != -1) {
					for (set<string>::iterator j = input[i->ret.first].begin(); j != input[i->ret.first].end(); j++) {
						if (output[cnt].insert(*j).second == true) {
							if (isDefine[cnt].count(*j) == 0) {
								input[cnt].insert(*j);
							}
							isChange = true;
						}
					}
				}
				if (i->ret.second != -1) {
					for (set<string>::iterator j = input[i->ret.second].begin(); j != input[i->ret.second].end(); j++) {
						if (output[cnt].insert(*j).second == true) {
							if (isDefine[cnt].count(*j) == 0) {
								input[cnt].insert(*j);
							}
							isChange = true;
						}
					}
				}
			}
		}
		blockInput[i->first] = input;
		blockOutput[i->first] = output;

		for (vector<BaseBlock>::iterator j = baseBlocks.begin(); j != baseBlocks.end(); j++) {
			BaseBlockWithInfo blockWithInfo;
			blockWithInfo.ret = j->ret;
			blockWithInfo.name = j->name;
			for (vector<Quaternion>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				blockWithInfo.qCodes.push_back(QuaternionAttachInfo(*k, StandBy_ActiveInfo(-1, false), StandBy_ActiveInfo(-1, false), StandBy_ActiveInfo(-1, false)));
			}
			baseBlocksWithInfo.push_back(blockWithInfo);
		}

		vector<map<string, StandBy_ActiveInfo> > symbolTables;

		for (vector<BaseBlock>::iterator j = baseBlocks.begin(); j != baseBlocks.end(); j++) {
			map<string, StandBy_ActiveInfo> symbolTable;

			for (vector<Quaternion>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				if (k->op == "j==" || k->op == "j!=" || k->op == "j<=" || k->op == "j<" || k->op == "j>=" || k->op == "j>") {
					if (isalpha(k->arg1[0])) {
						symbolTable[k->arg1] = StandBy_ActiveInfo(-1, false);
					}
					if (isalpha(k->arg2[0])) {
						symbolTable[k->arg2] = StandBy_ActiveInfo(-1, false);
					}
				}
				else if (k->op != "j" && k->op != "call") {
					if (isalpha(k->arg1[0])) {
						symbolTable[k->arg1] = StandBy_ActiveInfo(-1, false);
					}
					if (isalpha(k->arg2[0])) {
						symbolTable[k->arg2] = StandBy_ActiveInfo(-1, false);
					}
					if (isalpha(k->result[0])) {
						symbolTable[k->result] = StandBy_ActiveInfo(-1, false);
					}
				}
				
			}
			symbolTables.push_back(symbolTable);
		}

		int cnt = 0;
		for (vector<set<string> >::iterator j = output.begin(); j != output.end(); j++, cnt++) {
			for (set<string>::iterator k = j->begin(); k != j->end(); k++) {
				symbolTables[cnt][*k] = StandBy_ActiveInfo(-1, true);
			}
		}

		cnt = 0;
		for (vector<BaseBlockWithInfo>::iterator j = baseBlocksWithInfo.begin(); j != baseBlocksWithInfo.end(); j++, cnt++) {
			int endQcode = j->qCodes.size() - 1;
			for (vector<QuaternionAttachInfo>::reverse_iterator k = j->qCodes.rbegin(); k != j->qCodes.rend(); k++, endQcode--) {
				if (k->q.op == "j==" || k->q.op == "j!=" || k->q.op == "j<=" || k->q.op == "j<" || k->q.op == "j>=" || k->q.op == "j>") {
					if (isalpha(k->q.arg1[0])) {
						k->lop = symbolTables[cnt][k->q.arg1];
						symbolTables[cnt][k->q.arg1] = StandBy_ActiveInfo(endQcode, true);
					}
					if (isalpha(k->q.arg2[0])) {
						k->rop = symbolTables[cnt][k->q.arg2];
						symbolTables[cnt][k->q.arg2] = StandBy_ActiveInfo(endQcode, true);
					}
				}
				else if (k->q.op != "j" && k->q.op != "call") {
					if (isalpha(k->q.arg1[0])) {
						k->lop = symbolTables[cnt][k->q.arg1];
						symbolTables[cnt][k->q.arg1] = StandBy_ActiveInfo(endQcode, true);
					}
					if (isalpha(k->q.arg2[0])) {
						k->rop = symbolTables[cnt][k->q.arg2];
						symbolTables[cnt][k->q.arg2] = StandBy_ActiveInfo(endQcode, true);
					}
					if (isalpha(k->q.result[0])) {
						k->lres = symbolTables[cnt][k->q.result];
						symbolTables[cnt][k->q.result] = StandBy_ActiveInfo(-1, false);
					}
				}
			}
		}

		function_baseBlockWithInfo[i->first] = baseBlocksWithInfo;
	}

	return OK;
}

Status StandBy_ActiveInfo::outputStandBy_ActiveInfo() 
{
	cout << "(";
	if (num == -1) {
		cout << "^";
	}
	else {
		cout << num;
	}

	cout << ",";

	if (isActive) {
		cout << "Y";
	}
	else {
		cout << "^";
	}

	cout << ")";

	return OK;
}

Status ObjectAnalyst::outputBlocksAttachInfo()
{
	for (map<string, vector<BaseBlockWithInfo> >::iterator i = function_baseBlockWithInfo.begin(); i != function_baseBlockWithInfo.end(); i++) {
		cout << "[" << i->first << "]" << endl;
		for (vector<BaseBlockWithInfo>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			cout << j->name << ":" << endl;
			for (vector<QuaternionAttachInfo>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				cout << "\t(" << k->q.op << "," << k->q.arg1 << "," << k->q.arg2 << "," << k->q.result << ")";
				k->lop.outputStandBy_ActiveInfo();
				k->rop.outputStandBy_ActiveInfo();
				k->lres.outputStandBy_ActiveInfo();
				cout << endl;
			}
			cout << "\tnext1:" << j->ret.first << endl;
			cout << "\tnext2:" << j->ret.second << endl;
		}
		cout << endl;
	}
	return OK;
}