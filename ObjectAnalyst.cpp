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

		// 已经被定义过，已经被使用过
		vector<set<string> > isDefine, isUsed;

		// 输入，输出
		vector<set<string> > input, output;

		// 遍历所有 baseBlocks，依次插入
		for (vector<BaseBlock>::iterator j = baseBlocks.begin(); j != baseBlocks.end(); j++) {
			set<string> isdef, isuse;
			for (vector<Quaternion>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				if (k->op[0] == 'j') {
					if (isalpha(k->arg1[0]) && isdef.count(k->arg1) == 0) {
						isuse.insert(k->arg1); 
					}
					if (isalpha(k->arg2[0]) && isdef.count(k->arg2) == 0) {
						isuse.insert(k->arg2); 
					}
				}
				else if (k->op != "j" && k->op != "call") { 
					if (isalpha(k->arg1[0]) && isdef.count(k->arg1) == 0) {
						isuse.insert(k->arg1); 
					}
					if (isalpha(k->arg2[0]) && isdef.count(k->arg2) == 0) {
						isuse.insert(k->arg2); 
					}
					if (isalpha(k->result[0]) && isuse.count(k->result) == 0) {
						isdef.insert(k->result); 
					}
				}
			}
			input.push_back(isuse);
			isDefine.push_back(isdef);
			isUsed.push_back(isuse);
			output.push_back(set<string>()); // 插入一个空串
		}

		// 直至没有增加的时候跳出
		bool isChange = true;
		while (isChange) {
			isChange = false;
			int cnt = 0;
			// cout << "baseBlock长度为" << baseBlocks.size() << endl;
			for (vector<BaseBlock>::iterator i = baseBlocks.begin(); i != baseBlocks.end(); i++, cnt++) {
				if (i->ret.first != -1) {
					for (set<string>::iterator j = input[i->ret.first].begin(); j != input[i->ret.first].end(); j++) {
						if (output[cnt].insert(*j).second == true) {
							// cout << "插入了out1" << endl;
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
							// cout << "插入了out2" << endl;
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

		// cout << "当前map的大小为" << i->first << "->" << blockOutput.size() << endl;

		for (vector<BaseBlock>::iterator j = baseBlocks.begin(); j != baseBlocks.end(); j++) {
			BaseBlockWithInfo blockWithInfo;
			blockWithInfo.ret.first = j->ret.first;
			blockWithInfo.ret.second = j->ret.second;
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
		// cout << "output的大小为" << output.size() << endl;
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


Status ObjectAnalyst::productObjectCode_total()
{
	objectCode.push_back("lui $sp, 0x1001");
	objectCode.push_back("j main");
	for (map<string, vector<BaseBlockWithInfo> >::iterator i = function_baseBlockWithInfo.begin(); i != function_baseBlockWithInfo.end(); i++) {
		productObjectCode_baseBlockWithInfo(i);
	}
	objectCode.push_back("end:");
	return OK;
}

Status ObjectAnalyst::productObjectCode_baseBlockWithInfo(map<string, vector<BaseBlockWithInfo> >::iterator &i)
{
	variableStorge.clear();
	functionAnalysing = i->first;
	int index = 0;
	for (vector<BaseBlockWithInfo>::iterator j = i->second.begin(); j != i->second.end(); j++, index++) {
		baseBlockWithInfoAnalysing = j;
		productObjectCode_baseBlock(index);
	}
	return OK;
}

Status ObjectAnalyst::productObjectCode_baseBlock(int index)
{
	RVALUE.clear();
	AVALUE.clear();

	for (set<string>::iterator i = blockInput[functionAnalysing][index].begin(); i != blockInput[functionAnalysing][index].end(); i++) {
		AVALUE[*i].insert(*i);
	}

	registerList.clear();
	for (int cnt = 0; cnt < REGNUM; cnt++) {
		registerList.push_back("$s" + to_string(cnt));
	}

	objectCode.push_back(baseBlockWithInfoAnalysing->name + ":");
	if (index == 0) {
		if (functionAnalysing == "main") {
			stackTop = 8;
		}
		else {
			objectCode.push_back("sw $ra 4($sp)");
			stackTop = 8;
		}
	}

	int argNum = 0; // 实参
	int parNum = 0; // 形参
	list<pair<string, bool> > parList; // 参数列表

	for (vector<QuaternionAttachInfo>::iterator i = baseBlockWithInfoAnalysing->qCodes.begin(); i != baseBlockWithInfoAnalysing->qCodes.end(); i++) {
		quaternionAttachInfoAnalysing = i;
		if (i + 1 != baseBlockWithInfoAnalysing->qCodes.end()) {
			productObjectCode_quaternion(index, argNum, parNum, parList);
		}
		else {
			if (i->q.op[0] == 'j' || i->q.op == "call" || i->q.op == "get" || i->q.op == "return") {
				// cout << "调用了分支control" << endl;
				// cout << "当前分析的函数为" << functionAnalysing << "序号为" << index << endl;
				storeAllVariable(blockOutput[functionAnalysing][index]);
				productObjectCode_quaternion(index, argNum, parNum, parList);
			}
			else { 
				productObjectCode_quaternion(index, argNum, parNum, parList);
				storeAllVariable(blockOutput[functionAnalysing][index]);
			}
		}
	}

	return OK;
}

Status ObjectAnalyst::productObjectCode_quaternion(int index, int &argNum, int &parNum, list<pair<string, bool> > &parList)
{
	Quaternion q = quaternionAttachInfoAnalysing->q;
	if (q.op == "j") {
		objectCode.push_back("j " + q.result);
	}
	else if (q.op == "j==" || q.op == "j!=" || q.op == "j<=" || q.op == "j<" || q.op == "j>=" || q.op == "j>") {
		string op;
		if (q.op == "j==") {
			op = "beq";
		}
		else if (q.op == "j!=") {
			op = "bne";
		}
		else if (q.op == "j<=") {
			op = "ble";
		}
		else if (q.op == "j<") {
			op = "blt";
		}
		else if (q.op == "j>=") {
			op = "bge";
		}
		else if (q.op == "j>") {
			op = "bgt";
		}

		string arg1 = allocateRegister(q.arg1);
		string arg2 = allocateRegister(q.arg2);

		objectCode.push_back(op + " " + arg1 + " " + arg2 + " " + q.result);

		if (!quaternionAttachInfoAnalysing->lop.isActive) {
			freeVariable(q.arg1);
		}
		if (!quaternionAttachInfoAnalysing->rop.isActive) {
			freeVariable(q.arg2);
		}

	}
	else if (q.op == "par") {
		// cout << "lop" << quaternionAttachInfoAnalysing->lop.num << endl;
		parList.push_back(pair<string, bool>(q.arg1, quaternionAttachInfoAnalysing->lop.isActive));
	}
	else if (q.op == "call") {
		// cout << "parList的大小为" << parList.size() << endl;
		// exit(-1);
		for (list<pair<string, bool> >::iterator i = parList.begin(); i != parList.end(); i++) {
			string regPos = allocateRegister(i->first);
			// cout << "当前regPos为" << regPos << endl;
			objectCode.push_back("sw " + regPos + " " + to_string(stackTop + 4 * (++argNum + 1)) + "($sp)");
			if (!i->second) {
				freeVariable(i->first);
			}
		}

		objectCode.push_back("sw $sp " + to_string(stackTop) + "($sp)");
		objectCode.push_back("addi $sp $sp " + to_string(stackTop));

		objectCode.push_back("jal " + q.arg1);
		objectCode.push_back(string("lw $sp 0($sp)"));
	}
	else if (q.op == "return") {
		if (isdigit(q.arg1[0])) {
			objectCode.push_back("addi $v0 $zero " + q.arg1);
		}
		else if (isalpha(q.arg1[0])) {
			set<string>::iterator i = AVALUE[q.arg1].begin();
			if ((*i)[0] == '$') {
				objectCode.push_back("add $v0 $zero " + *i);
			}
			else {
				objectCode.push_back("lw $v0 " + to_string(variableStorge[*i]) + "($sp)");
			}
		}

		if (functionAnalysing == "main") {
			objectCode.push_back(string("j end"));
		}
		else {
			objectCode.push_back("lw $ra 4($sp)");
			objectCode.push_back("jr $ra");
		}
	}
	else if (q.op == "get") {
		variableStorge[q.result] = stackTop;
		stackTop += 4;
		AVALUE[q.result].insert(q.result);
	}
	else if (q.op == "=") {
		if (q.arg1 == "@RETURN_PLACE") {
			RVALUE["$v0"].insert(q.result);
			AVALUE[q.result].insert("$v0");
		}
		else {
			string ret = allocateRegister(q.arg1);
			RVALUE[ret].insert(q.result);
			AVALUE[q.result].insert(ret);
		}
	}
	else {
		string arg1 = allocateRegister(q.arg1);
		string arg2 = allocateRegister(q.arg2);
		string result = disRegister();
		if (q.op == "+") {
			objectCode.push_back("add " + result + " " + arg1 + " " + arg2);
		}
		else if (q.op == "-") {
			objectCode.push_back("sub " + result + " " + arg1 + " " + arg2);
		}
		else if (q.op == "*") {
			objectCode.push_back("mul " + result + " " + arg1 + " " + arg2);
		}
		else if (q.op == "/") {
			objectCode.push_back("div " + arg1 + " " + arg2);
			objectCode.push_back("mflo " + result);
		}

		if (!quaternionAttachInfoAnalysing->lop.isActive) {
			freeVariable(q.arg1);
		}
		if (!quaternionAttachInfoAnalysing->rop.isActive) {
			freeVariable(q.arg2);
		}
	}
	return OK;
}

Status ObjectAnalyst::storeAllVariable(set<string> &output)
{
	// cout << "output的大小为"<< "size1:"<< output.size() << endl;
	for (set<string>::iterator i = output.begin(); i != output.end(); i++) {
		// cout << *i << endl;
		string reg;
		bool isInSet = false;
		for (set<string>::iterator j = AVALUE[*i].begin(); j != AVALUE[*i].end(); j++) {
			if ((*j)[0] != '$') {
				isInSet = true;
				break;
			}
			else {
				reg = *j;
			}
		}

		if (isInSet == false) {
			storeVariable(reg, *i);
		}
	}

	return OK;
}

Status ObjectAnalyst::storeVariable(string reg, string variable)
{
	if (variableStorge.find(variable) != variableStorge.end()) {
		objectCode.push_back("sw " + reg + " " + to_string(variableStorge[variable]) + "($sp)");
	}
	else {
		variableStorge[variable] = stackTop;
		stackTop += 4;
		objectCode.push_back("sw " + reg + " " + to_string(variableStorge[variable]) + "($sp)");
	}
	AVALUE[variable].insert(variable);
	return OK;
}

string ObjectAnalyst::allocateRegister(string variable)
{
	if (isdigit(variable[0])) {
		string ret = allocateRegister();
		objectCode.push_back("addi " + ret + " $zero " + variable);
		return ret;
	}

	for (set<string>::iterator i = AVALUE[variable].begin(); i != AVALUE[variable].end(); i++) {
		if ((*i)[0] == '$') {
			return *i;
		}
		else {
			continue;
		}
	}

	string ret = allocateRegister();
	objectCode.push_back("lw " + ret + " " + to_string(variableStorge[variable]) + "($sp)");
	AVALUE[variable].insert(ret);
	RVALUE[ret].insert(variable);

	return ret;
}

string ObjectAnalyst::allocateRegister()
{
	string ret;
	if (registerList.size()) {
		ret = registerList.back();
		registerList.pop_back();
		return ret;
	}

	// 暂时还没有实现寄存器不够的情况
	cout << "还没做呢" << endl;
	exit(-1);
}

Status ObjectAnalyst::freeVariable(string variable)
{
	for (set<string>::iterator i = AVALUE[variable].begin(); i != AVALUE[variable].end(); i++) {
		if ((*i)[0] == '$') {
			RVALUE[*i].erase(variable);
			if (RVALUE[*i].size() == 0 && (*i)[1] == 's') {
				registerList.push_back(*i);
			}
		}
	}
	AVALUE[variable].clear();

	return OK;
}

string ObjectAnalyst::disRegister()
{
	if (!isdigit(quaternionAttachInfoAnalysing->q.arg1[0])) {
		set<string>& arg1 = AVALUE[quaternionAttachInfoAnalysing->q.arg1];
		for (set<string>::iterator i = arg1.begin(); i != arg1.end(); i++) {
			if ((*i)[0] == '$' && RVALUE[*i].size() == 1 && (quaternionAttachInfoAnalysing->q.result == quaternionAttachInfoAnalysing->q.arg1 || !quaternionAttachInfoAnalysing->lop.isActive)) {
				AVALUE[quaternionAttachInfoAnalysing->q.result].insert(*i);
				RVALUE[*i].insert(quaternionAttachInfoAnalysing->q.result);
				return (*i);
			}
		}
	}

	string ret = allocateRegister();
	AVALUE[quaternionAttachInfoAnalysing->q.result].insert(ret);
	RVALUE[ret].insert(quaternionAttachInfoAnalysing->q.result);
	return ret;
}

Status ObjectAnalyst::outputObjectCode()
{
	for (vector<string>::iterator i = objectCode.begin(); i != objectCode.end(); i++) {
		cout << *i << endl;
	}
	return OK;
}
