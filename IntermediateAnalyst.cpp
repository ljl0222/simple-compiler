#include "common.h"
#include "IntermediateAnalyst.h"

Quaternion::Quaternion()
{

}

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

Status IntermediateCode::divideBaseBlocks(vector<pair<int, string> > functionEnter)
{
	int L_temp = 1; // label的名字

	for (vector<pair<int, string> >::iterator i = functionEnter.begin(); i != functionEnter.end(); i++) 
	{
		vector<BaseBlock> baseBlocks; // 存储本次迭代所有的基本块

		// 这里每个基本块必定要按照从小到大的顺序，应当使用小顶堆
		priority_queue<int, vector<int>, greater<int> > blockEnter;
		blockEnter.push(i->first); // 入口

		// 从一个基本块开始往后找，依次判断指令
		int endPos;
		if (i + 1 == functionEnter.end()) { // 如果是最后一个指令，就到最后了
			endPos = result.size();
		}
		else { // 否则在下一个的指令的开头
			endPos = (i + 1)->first;
		}

		for (int cnt = i->first; cnt != endPos; cnt++) {
			if (result[cnt].op == "j") {
				blockEnter.push(atoi(result[cnt].result.c_str())); // 无条件转移语句转移到的语句
				// cout << "执行到了j" << endl;
			}
			else if (result[cnt].op == "j==" || result[cnt].op == "j!=" || result[cnt].op == "j>=" || result[cnt].op == "j>" || result[cnt].op == "j<=" || result[cnt].op == "j<") 
			{
				if (cnt + 1 < endPos) {
					blockEnter.push(cnt + 1); // 条件转移语句后面的一句
				}
				blockEnter.push(atoi(result[cnt].result.c_str())); // 条件转移语句转移到的语句
				// cout << "执行到了j=" << endl;
			}
			else if (result[cnt].op == "return" || result[cnt].op == "call") {
				if (cnt + 1 < endPos) {
					blockEnter.push(cnt + 1); // 条件转移语句后面的一句
				}
			}
		}

		// 这里进行基本块的划分
		BaseBlock baseBlock;
		map<int, string> label_enter; // 这里划分入口和标签
		map<int, int> enter_baseBlock;

		bool isFirst = true;

		int endBlockEnter = blockEnter.top(); // 上一个的末尾
		blockEnter.pop();

		while (1) {
			if (blockEnter.empty()) { // 如果已经检查完毕，则直接退出去
				break;
			}

			int block = blockEnter.top();
			blockEnter.pop();

			if (block == endBlockEnter) {
				continue;
			}

			for (int cnt = endBlockEnter; cnt < block; cnt++) { // 计算这个基本块里有多少四元式，全部复制进去
				baseBlock.qCodes.push_back(result[cnt]);
			}

			if (isFirst) {
				// 如果是第一块，就直接拿着函数名作为Label的名字
				baseBlock.name = i->second;
				isFirst = false;
			}
			else {
				baseBlock.name = "Label" + to_string(L_temp);
				L_temp++;
				label_enter[endBlockEnter] = baseBlock.name;
			}

			enter_baseBlock[endBlockEnter] = baseBlocks.size();
			baseBlocks.push_back(baseBlock);
			endBlockEnter = block;
			baseBlock.qCodes.clear();
		}

		// 出来再做一次
		if (isFirst) {
			baseBlock.name = i->second;
			isFirst = false;
		}
		else {
			baseBlock.name = "Label" + to_string(L_temp);
			L_temp++;
			label_enter[endBlockEnter] = baseBlock.name;
		}

		if (i + 1 != functionEnter.end()) {
			for (int cnt = endBlockEnter; cnt != (i + 1)->first; cnt++) {
				baseBlock.qCodes.push_back(result[cnt]);
			}
		}
		else {
			for (int cnt = endBlockEnter; cnt != result.size(); cnt++) {
				baseBlock.qCodes.push_back(result[cnt]);
			}
		}

		enter_baseBlock[endBlockEnter] = baseBlocks.size();
		baseBlocks.push_back(baseBlock);
		int cnt = 0;
		for (vector<BaseBlock>::iterator i = baseBlocks.begin(); i != baseBlocks.end(); i++, cnt++) {
			vector<Quaternion>::reverse_iterator r = i->qCodes.rbegin();
			if (r->op == "j") {
				i->ret.first = enter_baseBlock[atoi(r->result.c_str())];
				i->ret.second = -1;
				r->result = label_enter[atoi(r->result.c_str())];
			}
			else if (r->op == "j==" || r->op == "j!=" || r->op == "j>=" || r->op == "j>" || r->op == "j<=" || r->op == "j<") 
			{
				i->ret.first = cnt + 1;
				i->ret.second = enter_baseBlock[atoi(r->result.c_str())];
				if (i->ret.second == cnt + 1) {
					i->ret.second = -1;
				}
				r->result = label_enter[atoi(r->result.c_str())];
			}
			else if (r->op == "return") {
				i->ret.first = -1;
				i->ret.second = -1;
			}
			else {
				i->ret.first = cnt + 1;
				i->ret.second = -1;
			}
		}

		functionBlocks[i->second] = baseBlocks;

	}

	return OK;
}

Status IntermediateCode::outputBlocks()
{
	for (map<string, vector<BaseBlock> >::iterator i = functionBlocks.begin(); i != functionBlocks.end(); i++) {
		cout << "[" << i->first << "]" << endl;
		for (vector<BaseBlock>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			cout << j->name << ":" << endl;
			for (vector<Quaternion>::iterator k = j->qCodes.begin(); k != j->qCodes.end(); k++) {
				cout << "\t(" << k->op << "," << k->arg1 << "," << k->arg2 << "," << k->result << ")" << endl;
			}
			cout << "\tnext1:" << j->ret.first << endl;
			cout << "\tnext2:" << j->ret.second << endl;
		}
		cout << endl;
	}
	return OK;
}

map<string, vector<BaseBlock> >* IntermediateCode::getFunctionBlocks()
{
	return &functionBlocks;
}

