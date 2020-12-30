#include "ParAnalyst.h"

bool operator <(const Symbol& a, const Symbol& b)
{
	return a.content < b.content;
}

bool operator ==(const Symbol& a, const Symbol& b)
{
	return a.content == b.content;
}

bool operator <(const Item& a, const Item& b)
{
	if (a.idProduction == b.idProduction)
	{
		if (a.expect == b.expect)
		{
			return a.posPoint < b.posPoint;
		}
		else
			return a.expect < b.expect;
	}
	else
		return a.idProduction < b.idProduction;
}

bool operator ==(const Item& a, const Item& b)
{
	return a.idProduction == b.idProduction && a.expect == b.expect && a.posPoint == b.posPoint;
}


Symbol::Symbol(string content, bool Vt)
{
	this->content = content;
	this->Vt = Vt;
}

Symbol::Symbol(string content, bool Vt, string name)
{
	this->content = content;
	this->Vt = Vt;
	this->name = name;
}

Symbol::Symbol()
{
	// 这里啥也不做
}

SymbolID::SymbolID(Symbol s, string name) : Symbol(s)
{
	idName = name;
}

SymbolNum::SymbolNum(Symbol s, string num) : Symbol(s)
{
	numName = num;
}

Production::Production()
{
	// 这里啥也不做
}

Behavior::Behavior()
{
	// 这里啥也不做
}

Item::Item(int posPoint, Symbol expect, int idPro)
{
	this->posPoint = posPoint;
	this->expect = expect;
	this->idProduction = idPro;
}

Behavior::Behavior(Behave bh, int S)
{
	this->bh = bh;
	this->nextS = S;
}

Status ParAnalyst::openInputProductions(string path)
{
	inputProductions.open(path, ios::in);
	if (!inputProductions.is_open())
	{
		cerr << "打开输入文件" << path << "失败" << endl;
		exit(ERROR);
	}
	return OK;
}

ParAnalyst::ParAnalyst()
{
	openInputProductions("inputProductions.txt");
}

ParAnalyst::ParAnalyst(string path)
{
	openInputProductions(path);
}

// 读产生式
Status ParAnalyst::getProductions()
{
	int id = 0;
	string tempString;

	// 不断的读产生式
	while (inputProductions >> tempString)
	{
		Production tempProduction;
		tempProduction.id = id;
		tempProduction.lcontent = Symbol(tempString, false);
		inputProductions >> tempString; // 读掉assign语句
		char temp[MAXLINECNT];
		inputProductions.getline(temp, MAXLINECNT, '\n');
		stringstream s(temp);
		while (s >> tempString)
		{
			tempProduction.rcontent.push_back(Symbol(tempString, isVt(tempString)));
		}
		Productions.push_back(tempProduction); // 放回项目组当中
		id++;
	}

	return OK;
}

// 输出读到的产生式（test）
Status ParAnalyst::showProductions()
{
	cout << setw(10) << "ID" << setw(20) << "LEFT" << setw(10) << "   " << "RIGHT" << endl;
	vector<Production>::iterator itProduction;
	for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
	{
		cout << setw(10) << itProduction->id << "   " << setw(20) << itProduction->lcontent.content << "   ";
		vector<Symbol>::iterator itSymbol;
		for (itSymbol = itProduction->rcontent.begin(); itSymbol != itProduction->rcontent.end(); itSymbol++)
			cout << itSymbol->content << ' ';
		cout << endl;
	}
	return OK;
}

Status ParAnalyst::initFirst()
{
	bool isAddFirst = false;

	First[Symbol("#", true)].insert(Symbol("#", true)); // 这里将#加入

	// 这里，重复这样的过程直至First集合不发生变化
	while (1)
	{
		isAddFirst = false;
		// 这里需要遍历每个项目的每个Symbol
		vector<Production>::iterator itProduction;
		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
		{
			vector<Symbol>::iterator itSymbol;
			for (itSymbol = itProduction->rcontent.begin(); itSymbol != itProduction->rcontent.end(); itSymbol++)
			{
				/*
				 * 如果这里的右部符号是终结符，将其加入，直接结束遍历该项目的右部符号即可(break)
				 */
				if (isVt(itSymbol->content) == true)
				{
					if (First[itProduction->lcontent].insert(*itSymbol).second == true)
						isAddFirst = true;
					break;
				}

				/*
				 * 如果这里的右部符号是非终结符，需要分类
				 * 若起始符号有eps，继续遍历后面的符号，看看要不要加入left的first
				 * 若没有eps，完成加入这个右部符号后，直接结束遍历该项目的右部符号，转入下一个项目(break)
				 */
				else
				{
					bool isEps = false;
					set<Symbol>::iterator itFirst;

					// 遍历一个右部符号的first集合
					for (itFirst = First[*itSymbol].begin(); itFirst != First[*itSymbol].end(); itFirst++)
					{
						if (itFirst->content == "EPS")
						{
							isEps = true;
						}
						else // 这里else是为了不加入eps
						{
							if (First[itProduction->lcontent].insert(*itFirst).second == true)
								isAddFirst = true;
						}
					}

					// 如果没有eps，那么就不需要继续遍历右部符号串
					if (isEps == false)
						break;
				}


			} // end of 遍历项目

			// 如果这里，在这个项目能够遍历到最后一个符号，则说明需要加入eps作为该左部符号的First集合
			if (itSymbol == itProduction->rcontent.end())
			{
				if (First[itProduction->lcontent].insert(Symbol("EPS", true)).second == true)
					isAddFirst = true;
			}

		}// end of 遍历产生式

		// 如果First集不变，就跳出while
		if (!isAddFirst)
			break;
	}// end of while

	return OK;
}

//Status ParAnalyst::initFollow()
//{
//	bool isAddFollow = false;
//
//	// 首先将#加入起始符号的Follow集合
//	Follow[Productions[0].lcontent].insert(Symbol("#", true));
//
//	// 重复这样的过程直至Follow集不变（不增加）
//	while (1)
//	{
//		isAddFollow = false;
//
//		// 首先遍历每个项目的每个右部符号
//		vector<Production>::iterator itProduction;
//		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
//		{
//			vector<Symbol>::iterator itSymbol;
//			for (itSymbol = itProduction->rcontent.begin(); itSymbol != itProduction->rcontent.end(); itSymbol++)
//			{
//				// 这里，以itSymbol为基准，开始去找这个符号右边的符号
//				vector<Symbol>::iterator itNextSymbol;
//				// 遍历itSymbol的右部符号
//				for (itNextSymbol = itSymbol + 1; itNextSymbol != itProduction->rcontent.end(); itNextSymbol++)
//				{
//
//				}
//
//
//			}// end of 遍历右部符号
//		}// end of 遍历项目
//	}// end of while
//}

Status ParAnalyst::showFirst()
{
	map<Symbol, set<Symbol>>::iterator itFirst;
	for (itFirst = First.begin(); itFirst != First.end(); itFirst++)
	{
		cout << itFirst->first.content << "的First集合为{";
		set<Symbol>::iterator itSet;
		for (itSet = itFirst->second.begin(); itSet != itFirst->second.end(); itSet++)
			cout << itSet->content << ' ';
		cout << "}" << endl;
	}
	return OK;
}

set<Symbol> ParAnalyst::getExpect(list<Symbol> S)
{
	set<Symbol> retSet;
	bool isAddFirst = false;

	while (1)
	{
		isAddFirst = false;
		list<Symbol>::iterator itList;
		for (itList = S.begin(); itList != S.end(); itList++)
		{
			/*
			 * 如果这里的右部符号是终结符，将其加入，直接结束遍历该项目的右部符号即可(break)
			 */
			if (isVt(itList->content) == true)
			{
				if (retSet.insert(*itList).second == true)
					isAddFirst = true;
				break;
			}

			/*
			 * 如果这里的右部符号是非终结符，需要分类
			 * 若起始符号有eps，继续遍历后面的符号，看看要不要加入left的first
			 * 若没有eps，完成加入这个右部符号后，直接结束遍历该项目的右部符号，转入下一个项目(break)
			 */
			else
			{
				bool isEps = false;
				set<Symbol>::iterator itFirst;

				// 遍历一个右部符号的first集合
				for (itFirst = First[*itList].begin(); itFirst != First[*itList].end(); itFirst++)
				{
					if (itFirst->content == "EPS")
					{
						isEps = true;
					}
					else // 这里else是为了不加入eps
					{
						if (retSet.insert(*itFirst).second == true)
							isAddFirst = true;
					}
				}

				// 如果没有eps，那么就不需要继续遍历右部符号串
				if (isEps == false)
					break;
			}
		}// end of 遍历展望串
		// 如果这里，在这个项目能够遍历到最后一个符号，则说明需要加入eps作为该左部符号的First集合
		if (itList == S.end())
		{
			if (retSet.insert(Symbol("EPS", true)).second == true)
				isAddFirst = true;
		}

		// 如果First集不变，就跳出while
		if (!isAddFirst)
			break;
	}// end of while

	return retSet;
}

// 由一个状态产生一个状态集合（其实是求闭包的过程？
I ParAnalyst::Closure(Item item)
{
	I ret;

	// 如果是规约项目
	// 如果点后面是终结符
	if (item.posPoint == Productions[item.idProduction].rcontent.size() || isVt(Productions[item.idProduction].rcontent[item.posPoint].content))
	{
		ret.Items.insert(item);
	}
	// 点后面是非终结符
	else
	{
		ret.Items.insert(item);
		vector<Production>::iterator itProduction;
		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
		{
			if (itProduction->lcontent == Productions[item.idProduction].rcontent[item.posPoint])
			{
				// 这里需要将找到的产生式都加入I，但是这里需要求一下展望符
				list<Symbol> SymList; //贝塔 a

				// 可能size会有问题
				// 将右边的产生式依次放入SymList，作为获得展望串的入口参数
				for (int cnt = 1; item.posPoint + cnt < (int)(Productions[item.idProduction].rcontent.size()); cnt++)
				{
					SymList.push_back(Productions[item.idProduction].rcontent[item.posPoint + cnt]);
				}
				SymList.push_back(item.expect); //记得放a

				set<Symbol> retSet; //获得的展望串
				retSet = getExpect(SymList);

				// 将这些展望符依次加入I（ret）中
				set<Symbol>::iterator itSymbol;
				for (itSymbol = retSet.begin(); itSymbol != retSet.end(); itSymbol++)
				{
					I temp = Closure(Item(0, *itSymbol, itProduction->id));

					set<Item>::iterator itItem;
					for (itItem = temp.Items.begin(); itItem != temp.Items.end(); itItem++)
					{
						if (ret.Items.insert(*itItem).second == 0)
						{
							//cout << "重复插入同样的元素" << endl;
							//cout << itItem->idProduction << endl;
							//cout << itItem->expect.content << endl;
							//cout << itItem->posPoint << endl;
						}

					}
				}
			}
		}
	}

	return ret;
}

Status ParAnalyst::createDFA()
{
	cout << "创建文法的LR(1)表中，请稍候，这里不是卡死了..." << endl;

	// 首先先新建好I0状态，从I0状态出发，去看别的状态
	I itemp = Closure(Item(0, Symbol("#", true), 0));
	DFA.status.push_back(itemp);

	int idSNow = 0;
	// 遍历每一个状态的每一个项目
	list<I>::iterator itStatus;
	for (itStatus = DFA.status.begin(); itStatus != DFA.status.end(); itStatus++, idSNow++)
	{
		set<Item>::iterator itItem;
		int cnt = 0;
		for (itItem = itStatus->Items.begin(), cnt = 0; itItem != itStatus->Items.end(); itItem++, cnt++)
		{
			// 若为规约项目（点在最后）
			if (Productions[itItem->idProduction].rcontent.size() == itItem->posPoint)
			{
				GOTO tempGOTO(idSNow, itItem->expect);
				if (itItem->idProduction == 0) // 用第一个拓广文法规约
				{
					ActionTb[tempGOTO] = Behavior(accept, itItem->idProduction);
				}
				else
				{
					ActionTb[tempGOTO] = Behavior(reduct, itItem->idProduction);
				}
			}
			// 若为移进项目 或者 读到了非终结符号
			else
			{
				// 取出点后面的符号
				Symbol nextSymbol = Productions[itItem->idProduction].rcontent[itItem->posPoint];

				// 先判断是否已经有之前的项目找过了对应的nextSymbol的状态，如果有就直接去找别的项目
				if (DFA.gotos.count(GOTO(idSNow, nextSymbol)) == 1)
					continue;

				// 否则就产生新状态
				I StatusNew = Closure(Item(itItem->posPoint + 1, itItem->expect, itItem->idProduction));

				// 这里需要看一眼下面的项目的nextSymbol是不是也是一样的，如果是，把那个项目的闭包同样加入新状态，他们是同一种移进
				if (itItem != itStatus->Items.end())
				{
					set<Item>::iterator nextItItem = itItem;
					nextItItem++;
					for (; nextItItem != itStatus->Items.end(); nextItItem++)
					{
						if (Productions[nextItItem->idProduction].rcontent.size() == nextItItem->posPoint)
							continue;
						// 若这个状态之后的项目也是移进nextSymbol
						else if (Productions[nextItItem->idProduction].rcontent[nextItItem->posPoint] == nextSymbol)
						{
							I StatusTemp = Closure(Item(nextItItem->posPoint + 1, nextItItem->expect, nextItItem->idProduction));
							StatusNew.Items.insert(StatusTemp.Items.begin(), StatusTemp.Items.end());
						}
					}
				}

				// 这里需要看一下是不是已经有了一样的状态
				list<I>::iterator itHadS;
				int idS_ = 0;
				bool isFound = false;
				// 遍历已有状态，看看是不是已经出现了一样的状态
				for (itHadS = DFA.status.begin(); itHadS != DFA.status.end(); itHadS++, idS_++)
				{
					// 如果已经有这样的状态了，就将当前状态读到nextSymbol指向已有的状态
					if (itHadS->Items == StatusNew.Items)
					{
						DFA.gotos[GOTO(idSNow, nextSymbol)] = idS_;
						if (isVt(nextSymbol.content)) // 假如是终结符
							ActionTb[GOTO(idSNow, nextSymbol)] = Behavior(shift, idS_);
						else // 假如是非终结符，这里更新gototb
							GotoTb[GOTO(idSNow, nextSymbol)] = idS_;
						isFound = true;
						break;
					}
				}

				// 如果是新状态
				if (isFound == false)
				{
					DFA.status.push_back(StatusNew);
					DFA.gotos[GOTO(idSNow, nextSymbol)] = DFA.status.size() - 1;
					if (isVt(nextSymbol.content))
						ActionTb[GOTO(idSNow, nextSymbol)] = Behavior(shift, DFA.status.size() - 1);
					else
						GotoTb[GOTO(idSNow, nextSymbol)] = DFA.status.size() - 1;
				}

			}// end of else

		}// end of 遍历项目
	}// end of 遍历状态





	//int cntStatus = 0;
	//int cntItem = 0;
	//// test
	//cout << endl;
	//list<I>::iterator itI;
	//for (itI = DFA.status.begin(); itI != DFA.status.end(); itI++)
	//{
	//	set<Item>::iterator itS;
	//	for (itS = itI->Items.begin(); itS != itI->Items.end(); itS++)
	//	{
	//		cout << "(" << itS->idProduction << "," << itS->expect.content << "," << itS->posPoint << ")" << endl;
	//		cout << "end of Items" << endl;
	//		cntItem++;
	//	}
	//	cout << "end of Status" << endl;
	//	cntStatus++;
	//}
	//cout << "共有" << cntItem << "个项目" << endl;
	//cout << "共有" << cntStatus << "个状态" << endl;



	return OK;
}

Status ParAnalyst::outputAction()
{
	cout << "***********************************************Action***********************************************" << endl;
	map<GOTO, Behavior>::iterator itMap;
	int cnt = 0;
	for (cnt = 0, itMap = ActionTb.begin(); itMap != ActionTb.end(); itMap++, cnt++)
	{
		cout << cnt << "   " << itMap->first.first << "   " << itMap->first.second.content << "   ";
		if (itMap->second.bh == 0)
			cout << "s";
		else if (itMap->second.bh == 1)
			cout << "r";
		else if (itMap->second.bh == 2)
			cout << "acc";
		else
			cout << "error";
		cout << "   " << itMap->second.nextS << endl;
	}
	return OK;
}

Status ParAnalyst::outputActionToFile()
{
	ofstream out;
	out.open("outputAction.txt", ios::out);
	if (!out.is_open())
	{
		cerr << "打开输出文件outputAction.txt失败" << endl;
		exit(-1);
	}

	out << "***********************************************Action***********************************************" << endl;
	map<GOTO, Behavior>::iterator itMap;
	int cnt = 0;
	for (cnt = 0, itMap = ActionTb.begin(); itMap != ActionTb.end(); itMap++, cnt++)
	{
		out << cnt << "   " << itMap->first.first << "   " << itMap->first.second.content << "   ";
		if (itMap->second.bh == 0)
			out << "shift";
		else if (itMap->second.bh == 1)
			out << "reduct";
		else if (itMap->second.bh == 2)
			out << "acc";
		else
			out << "error";
		out << "   " << itMap->second.nextS << endl;
	}

	out.close();
	return OK;
}

Status ParAnalyst::outputGoto()
{
	cout << "***********************************************Goto***********************************************" << endl;
	map<GOTO, int>::iterator itMap;
	int cnt = 0;
	for (cnt = 0, itMap = GotoTb.begin(); itMap != GotoTb.end(); itMap++, cnt++)
	{
		cout << cnt << "   " << itMap->first.first << "   " << itMap->first.second.content << "   " << itMap->second << endl;
	}
	return OK;
}

Status ParAnalyst::outputGotoToFile()
{
	ofstream out;
	out.open("outputGoto.txt", ios::out);
	if (!out.is_open())
	{
		cerr << "打开输出文件outputGoto.txt失败" << endl;
		exit(-1);
	}
	out << "***********************************************Goto***********************************************" << endl;
	map<GOTO, int>::iterator itMap;
	int cnt = 0;
	for (cnt = 0, itMap = GotoTb.begin(); itMap != GotoTb.end(); itMap++, cnt++)
	{
		out << cnt << "   " << itMap->first.first << "   " << itMap->first.second.content << "   " << itMap->second << endl;
	}
	out.close();
	return OK;
}

Status ParAnalyst::outputStatusStack()
{
	stack<int> S = StatusStack;
	cout << "StatusStack:";
	for (int cnt = 0; !S.empty(); cnt++)
	{
		cout << S.top() << ' ';
		S.pop();
	}
	cout << endl;
	return OK;
}

Status ParAnalyst::outputSymbolStack()
{
	stack<Symbol> S = SymbolStack;
	cout << "SymbolStack:";
	for (int cnt = 0; !S.empty(); cnt++)
	{
		cout << S.top().content << ' ';
		S.pop();
	}
	cout << endl;
	return OK;
}

Status ParAnalyst::outputStack(int cnt)
{
	cout << "***********************栈空间" << cnt << "***********************" << endl;
	this->outputStatusStack();
	this->outputSymbolStack();
	return OK;
}

Status ParAnalyst::outputStackToFile(int cnt)
{
	
	ofstream out;

	if (cnt == 0)
		out.open("outputStack.txt", ios::ate);
	else
		out.open("outputStack.txt", ios::app);

	if (!out.is_open())
	{
		cerr << "打开输出文件outputStack.txt失败" << endl;
		exit(-1);
	}

	out << "***********************栈空间" << cnt << "***********************" << endl;

	stack<int> S0 = StatusStack;
	out << "StatusStack:";
	for (int cnt = 0; !S0.empty(); cnt++)
	{
		out << S0.top() << ' ';
		S0.pop();
	}
	out << endl;

	stack<Symbol> S1 = SymbolStack;
	out << "SymbolStack:";
	for (int cnt = 0; !S1.empty(); cnt++)
	{
		out << S1.top().content << ' ';
		S1.pop();
	}
	out << endl;

	out.close();
	return OK;
}

Symbol ParAnalyst::pop()
{
	Symbol s = SymbolStack.top();
	SymbolStack.pop();
	StatusStack.pop();
	return s;
}

Status ParAnalyst::LRAnalyse(list<Token> LexRes)
{
	SymbolStack.push(Symbol("#", true));
	StatusStack.push(0);

	int cnt = 0;

	int T_temp = 0;

	list<Token>::iterator itToken = LexRes.begin();
	while (itToken != LexRes.end())
	{
		this->outputStack(cnt);
		this->outputStackToFile(cnt);
		cnt++;

		// 如果读到注释，就继续
		if (itToken->first == LCOMMENT || itToken->first == LPCOMMENT || itToken->first == RPCOMMENT || itToken->first == PCOMMENT)
		{
			itToken++;
			continue;
		}

		//if (itToken->first == ENDFILE)
		//	break;

		Symbol next;
		if (itToken->first == ID)
			next = Symbol("ID", true, itToken->second);
		else if (itToken->first == NUM)
			next = Symbol("num", true, itToken->second);
		else
			next = Symbol(itToken->second, true);

		Behavior Bh = ActionTb[GOTO(StatusStack.top(), next)];

		// 开始LR分析，利用两个栈
		if (Bh.bh == shift) // 若为移进
		{
			SymbolStack.push(next);
			StatusStack.push(Bh.nextS);
			itToken++;
			continue;
		}

		else if (Bh.bh == reduct) // 若为归约
		{
			Production p = Productions[Bh.nextS];
			int popNum = p.rcontent.size();

			Symbol Symbolpushed = p.lcontent;

			// 弹出阶段，顺便产生一下中间代码
			if (Bh.nextS == 0) // P ::= N declare_list
			{
				Symbol _declare_list = pop();
				// Symbol _N = pop(); 
				// 这里我觉得是这样子的
				// 首先我们这里的N用于定义函数调用间的关系，找到函数出口
				// 这里由于我们不需要考虑子函数的过程调用，因此我觉得似乎不需要这里的N，修改产生式
				// 这里修改产生式为P ::= declare_list
			}
			else if (Bh.nextS == 1) // declare_list ::= declare declare_list
			{
				Symbol _declare_list = pop();
				Symbol _declare = pop();
			}
			else if (Bh.nextS == 2) // declare_list ::= declare
			{
				Symbol _declare = pop();
			}
			else if (Bh.nextS == 3) // declare ::= int ID M A function_declare
			{
				Symbol _function_declare = pop();
				Symbol _A = pop();
				Symbol _M = pop();
				Symbol _ID = pop();
				Symbol _int = pop();
			}
			else if (Bh.nextS == 4) // declare ::= int ID var_declare
			{
				Symbol _var_declare = pop();
				Symbol _ID = pop();
				Symbol _int = pop();
			}
			else if (Bh.nextS == 5) // declare ::= void ID M A function_declare
			{
				Symbol _function_declare = pop();
				Symbol _A = pop();
				Symbol _M = pop();
				Symbol _ID = pop();
				Symbol _void = pop();
			}
			else if (Bh.nextS == 6) // A ::= 
			{

			}
			else if (Bh.nextS == 7) // var_declare ::= ;
			{
				Symbol _semicolon = pop();
			}
			else if (Bh.nextS == 8) // function_declare ::= ( parameter ) sentence_block
			{
				Symbol _sectence_block = pop();
				Symbol _rbracket = pop();
				Symbol _parameter = pop();
				Symbol _lbracket = pop();
				// 函数
			}
			else if (Bh.nextS == 9) // parameter ::= parameter_list
			{
				Symbol _parameter_list = pop();
				// 函数
			}
			else if (Bh.nextS == 10) // parameter ::= 
			{
				// 函数
			}
			else if (Bh.nextS == 11) // parameter_list :: = param
			{
				Symbol _param = pop();
				// 函数
			}
			else if (Bh.nextS == 12) // parameter_list ::= param , parameter_list
			{
				Symbol _parameter_list = pop();
				Symbol _comma = pop();
				Symbol _param = pop(); 
				// 函数
			}
			else if (Bh.nextS == 13) // param ::= int ID
			{
				Symbol _ID = pop();
				Symbol _int = pop();
				code.emit(Quaternion("get", "_", "_", _ID.name));
			}
			else if (Bh.nextS == 14) // sentence_block ::= { inner_declare sentence_list }
			{
				Symbol _rbrace = pop();
				Symbol _sentence_list = pop();
				Symbol _inner_declare = pop();
				Symbol _lbrace = pop();
				Symbolpushed.nextlist = _sentence_list.nextlist;
			}
			else if (Bh.nextS == 15) // inner_declare ::= 
			{

			}
			else if (Bh.nextS == 16) // inner_declare ::= inner_var_declare ; inner_declare
			{
				Symbol _inner_declare = pop();
				Symbol _semicolon = pop();
				Symbol _inner_var_decalre = pop();
			}
			else if (Bh.nextS == 17) // inner_var_declare ::= int ID
			{
				Symbol _ID = pop();
				Symbol _int = pop();
			}
			else if (Bh.nextS == 18) // sentence_list ::= sentence M sentence_list
			{
				Symbol _sentence_list = pop();
				Symbol _M = pop();
				Symbol _sentence = pop();
				Symbolpushed.nextlist = _sentence_list.nextlist;
				code.backpatch(_sentence.nextlist, _M.quad);
			}
			else if (Bh.nextS == 19) // sentence_list :: = sentence
			{
				Symbol _sentence = pop();
				Symbolpushed.nextlist = _sentence.nextlist;
			}
			else if (Bh.nextS == 20) // sentence ::= if_sentence
			{
				Symbol _if_sentence = pop();
				Symbolpushed.nextlist = _if_sentence.nextlist;
			}
			else if (Bh.nextS == 21) // sentence ::= while_sentence
			{
				Symbol _while_sentence = pop();
				Symbolpushed.nextlist = _while_sentence.nextlist;
			}
			else if (Bh.nextS == 22) // sentence ::= return_sentence
			{
				Symbol _return_sentence = pop();
			}
			else if (Bh.nextS == 23) // sentence ::= assign_sentence
			{
				Symbol _assign_sentence = pop();
			}
			else if (Bh.nextS == 24) // assign_sentence ::= ID = expression ;
			{
				Symbol _semicolon = pop();
				Symbol _expression = pop();
				Symbol _assign = pop();
				Symbol _ID = pop();
				code.emit(Quaternion("=", _expression.name, "_", _ID.name));
			}
			else if (Bh.nextS == 25) // return_sentence ::= return ;
			{
				Symbol _semicolon = pop();
				Symbol _return = pop();
				code.emit(Quaternion("return", "_", "_", "_"));
			}
			else if (Bh.nextS == 26) // return_sentence ::= return expression ;    
			{
				Symbol _semicolon = pop();
				Symbol _expression = pop();
				Symbol _return = pop();
				code.emit(Quaternion("return", _expression.name, "_", "_"));
			}
			else if (Bh.nextS == 27) // while_sentence ::= while M ( expression ) A sentence_block 
			{
				Symbol _sentence_block = pop();
				Symbol _A = pop();
				Symbol _rbracket = pop();
				Symbol _expression = pop();
				Symbol _lbracket = pop();
				Symbol _M = pop();
				Symbol _while = pop();
				code.backpatch(_sentence_block.nextlist, _M.quad);
				Symbolpushed.nextlist = _expression.falselist;
				code.emit(Quaternion("j", "_", "_", to_string(_M.quad)));
			}
			else if (Bh.nextS == 28) // if_sentence ::= if ( expression ) A sentence_block    
			{
				Symbol _sentence_block = pop();
				Symbol _A = pop();
				Symbol _rbracket = pop();
				Symbol _expression = pop();
				Symbol _lbracket = pop();
				Symbol _if = pop();
				_expression.falselist.splice(_expression.falselist.begin(), _sentence_block.nextlist);
				Symbolpushed.nextlist = _expression.falselist;
			}
			else if (Bh.nextS == 29) // if_sentence ::= if ( expression ) A sentence_block N else M A sentence_block
			{
				Symbol _sentence_block1 = pop();
				Symbol _A1 = pop();
				Symbol _M = pop();
				Symbol _else = pop();
				Symbol _N = pop();
				Symbol _sentence_block2 = pop();
				Symbol _A2 = pop();
				Symbol _rbracket = pop();
				Symbol _expression = pop();
				Symbol _lbracket = pop();
				Symbol _if = pop();
				code.backpatch(_expression.falselist, _M.quad);
				Symbolpushed.nextlist = merge(_sentence_block2.nextlist, _sentence_block1.nextlist);
				Symbolpushed.nextlist = merge(Symbolpushed.nextlist, _N.nextlist);
			}
			else if (Bh.nextS == 30) // N ::=  
			{
			;	Symbolpushed.nextlist.push_back(code.nextquad());
				code.emit(Quaternion("j", "_", "_", "-1"));
			}
			else if (Bh.nextS == 31) // M ::=   
			{
			;	Symbolpushed.quad = code.nextquad();
			}
			else if (Bh.nextS == 32) // expression ::= add_expression    
			{
				Symbol _add_expressio = pop();
				Symbolpushed.name = _add_expressio.name;
			}
			else if (Bh.nextS == 33) // expression ::= add_expression > add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _gt = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j<=", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 34) // expression ::= add_expression < add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _lt = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j>=", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 35) // expression ::= add_expression == add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _equal = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j!=", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 36) // expression ::= add_expression >= add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _gte = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j<", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 37) // expression ::= add_expression <= add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _lte = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j>", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 38) // expression ::= add_expression != add_expression    
			{
				Symbol _add_expressio1 = pop();
				Symbol _ne = pop();
				Symbol _add_expressio2 = pop();
				Symbolpushed.falselist.push_back(code.nextquad());
				code.emit(Quaternion("j==", _add_expressio2.name, _add_expressio1.name, "-1"));
			}
			else if (Bh.nextS == 39) // add_expression ::= item    
			{
				Symbol _item = pop();
				Symbolpushed.name = _item.name;
			}
			else if (Bh.nextS == 40) // add_expression ::= item + add_expression    
			{
				Symbol _add_expressio = pop();
				Symbol _plus = pop();
				Symbol _item = pop();
				Symbolpushed.name = "T" + to_string(T_temp);
				T_temp++;
				code.emit(Quaternion("+", _item.name, _add_expressio.name, Symbolpushed.name));
			}
			else if (Bh.nextS == 41) // add_expression ::= item - add_expression    
			{
				Symbol _add_expressio = pop();
				Symbol _minus = pop();
				Symbol _item = pop();
				Symbolpushed.name = "T" + to_string(T_temp);
				T_temp++;
				code.emit(Quaternion("-", _item.name, _add_expressio.name, Symbolpushed.name));
			}
			else if (Bh.nextS == 42) // item ::= factor    
			{
				Symbol _factor = pop();
				Symbolpushed.name = _factor.name;
			}
			else if (Bh.nextS == 43) // item ::= factor * item    
			{
				Symbol _item = pop();
				Symbol _mul = pop();
				Symbol _factor = pop();
				Symbolpushed.name = "T" + to_string(T_temp);
				T_temp++;
				code.emit(Quaternion("*", _factor.name, _item.name, Symbolpushed.name));
			}
			else if (Bh.nextS == 44) // item ::= factor / item    
			{
				Symbol _item = pop();
				Symbol _div = pop();
				Symbol _factor = pop();
				Symbolpushed.name = "T" + to_string(T_temp);
				T_temp++;
				code.emit(Quaternion("/", _factor.name, _item.name, Symbolpushed.name));
			}
			else if (Bh.nextS == 45) // factor ::= num    
			{
				Symbol _num = pop();
				Symbolpushed.name = _num.name;
			}
			else if (Bh.nextS == 46) // factor ::= ( expression )    
			{
				Symbol _rbracket = pop();
				Symbol _expression = pop();
				Symbol _lbracket = pop();
				Symbolpushed.name = _expression.name;
			}
			else if (Bh.nextS == 47) // factor ::= ID ( argument_list )    
			{
				Symbol _rbracket = pop();
				Symbol _argument_list = pop();
				Symbol _lbracket = pop();
				Symbol _ID = pop();
				// 函数
			}
			else if (Bh.nextS == 48) // factor ::= ID     
			{
				Symbol _ID = pop();
				Symbolpushed.name = _ID.name;
			}
			else if (Bh.nextS == 49) // argument_list ::=     
			{
				// 函数
			}
			else if (Bh.nextS == 50) // argument_list ::= expression    
			{
				Symbol _expression = pop();
				// 函数
			}
			else if (Bh.nextS == 51) // argument_list ::= expression , argument_list    
			{
				Symbol _argument_list = pop();
				Symbol _comma = pop();
				Symbol _expression = pop();
				// 函数
			}
			else
			{
				// 弹出对应栈顶元素popNum个
				for (int cnt = 0; cnt < popNum; cnt++)
				{
					SymbolStack.pop();
					StatusStack.pop();
				}
				
			}

			// 压入对应的Goto表中的状态，归约式的左端
			SymbolStack.push(Symbolpushed);
			StatusStack.push(GotoTb[GOTO(StatusStack.top(), p.lcontent)]);
		}

		else if (Bh.bh == accept) // 接受
		{
			cout << "accept!" << endl;
			cout << "成功归约出目标结果，程序结束" << endl;
			code.outputCode();
			code.outputCodeToFile();
			break;
		}

		else // error
		{
			cout << "归约过程出现错误，已跳出，请检查你的输入程序，栈断点值已经保留在outputStack.txt中" << endl;
			break;
		}

	}

	return OK;
}

