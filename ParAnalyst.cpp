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
	// ����ɶҲ����
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
	// ����ɶҲ����
}

Behavior::Behavior()
{
	// ����ɶҲ����
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
		cerr << "�������ļ�" << path << "ʧ��" << endl;
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

// ������ʽ
Status ParAnalyst::getProductions()
{
	int id = 0;
	string tempString;

	// ���ϵĶ�����ʽ
	while (inputProductions >> tempString)
	{
		Production tempProduction;
		tempProduction.id = id;
		tempProduction.lcontent = Symbol(tempString, false);
		inputProductions >> tempString; // ����assign���
		char temp[MAXLINECNT];
		inputProductions.getline(temp, MAXLINECNT, '\n');
		stringstream s(temp);
		while (s >> tempString)
		{
			tempProduction.rcontent.push_back(Symbol(tempString, isVt(tempString)));
		}
		Productions.push_back(tempProduction); // �Ż���Ŀ�鵱��
		id++;
	}

	return OK;
}

// ��������Ĳ���ʽ��test��
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

	First[Symbol("#", true)].insert(Symbol("#", true)); // ���ｫ#����

	// ����ظ������Ĺ���ֱ��First���ϲ������仯
	while (1)
	{
		isAddFirst = false;
		// ������Ҫ����ÿ����Ŀ��ÿ��Symbol
		vector<Production>::iterator itProduction;
		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
		{
			vector<Symbol>::iterator itSymbol;
			for (itSymbol = itProduction->rcontent.begin(); itSymbol != itProduction->rcontent.end(); itSymbol++)
			{
				/*
				 * ���������Ҳ��������ս����������룬ֱ�ӽ�����������Ŀ���Ҳ����ż���(break)
				 */
				if (isVt(itSymbol->content) == true)
				{
					if (First[itProduction->lcontent].insert(*itSymbol).second == true)
						isAddFirst = true;
					break;
				}

				/*
				 * ���������Ҳ������Ƿ��ս������Ҫ����
				 * ����ʼ������eps��������������ķ��ţ�����Ҫ��Ҫ����left��first
				 * ��û��eps����ɼ�������Ҳ����ź�ֱ�ӽ�����������Ŀ���Ҳ����ţ�ת����һ����Ŀ(break)
				 */
				else
				{
					bool isEps = false;
					set<Symbol>::iterator itFirst;

					// ����һ���Ҳ����ŵ�first����
					for (itFirst = First[*itSymbol].begin(); itFirst != First[*itSymbol].end(); itFirst++)
					{
						if (itFirst->content == "EPS")
						{
							isEps = true;
						}
						else // ����else��Ϊ�˲�����eps
						{
							if (First[itProduction->lcontent].insert(*itFirst).second == true)
								isAddFirst = true;
						}
					}

					// ���û��eps����ô�Ͳ���Ҫ���������Ҳ����Ŵ�
					if (isEps == false)
						break;
				}


			} // end of ������Ŀ

			// �������������Ŀ�ܹ����������һ�����ţ���˵����Ҫ����eps��Ϊ���󲿷��ŵ�First����
			if (itSymbol == itProduction->rcontent.end())
			{
				if (First[itProduction->lcontent].insert(Symbol("EPS", true)).second == true)
					isAddFirst = true;
			}

		}// end of ��������ʽ

		// ���First�����䣬������while
		if (!isAddFirst)
			break;
	}// end of while

	return OK;
}

//Status ParAnalyst::initFollow()
//{
//	bool isAddFollow = false;
//
//	// ���Ƚ�#������ʼ���ŵ�Follow����
//	Follow[Productions[0].lcontent].insert(Symbol("#", true));
//
//	// �ظ������Ĺ���ֱ��Follow�����䣨�����ӣ�
//	while (1)
//	{
//		isAddFollow = false;
//
//		// ���ȱ���ÿ����Ŀ��ÿ���Ҳ�����
//		vector<Production>::iterator itProduction;
//		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
//		{
//			vector<Symbol>::iterator itSymbol;
//			for (itSymbol = itProduction->rcontent.begin(); itSymbol != itProduction->rcontent.end(); itSymbol++)
//			{
//				// �����itSymbolΪ��׼����ʼȥ����������ұߵķ���
//				vector<Symbol>::iterator itNextSymbol;
//				// ����itSymbol���Ҳ�����
//				for (itNextSymbol = itSymbol + 1; itNextSymbol != itProduction->rcontent.end(); itNextSymbol++)
//				{
//
//				}
//
//
//			}// end of �����Ҳ�����
//		}// end of ������Ŀ
//	}// end of while
//}

Status ParAnalyst::showFirst()
{
	map<Symbol, set<Symbol>>::iterator itFirst;
	for (itFirst = First.begin(); itFirst != First.end(); itFirst++)
	{
		cout << itFirst->first.content << "��First����Ϊ{";
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
			 * ���������Ҳ��������ս����������룬ֱ�ӽ�����������Ŀ���Ҳ����ż���(break)
			 */
			if (isVt(itList->content) == true)
			{
				if (retSet.insert(*itList).second == true)
					isAddFirst = true;
				break;
			}

			/*
			 * ���������Ҳ������Ƿ��ս������Ҫ����
			 * ����ʼ������eps��������������ķ��ţ�����Ҫ��Ҫ����left��first
			 * ��û��eps����ɼ�������Ҳ����ź�ֱ�ӽ�����������Ŀ���Ҳ����ţ�ת����һ����Ŀ(break)
			 */
			else
			{
				bool isEps = false;
				set<Symbol>::iterator itFirst;

				// ����һ���Ҳ����ŵ�first����
				for (itFirst = First[*itList].begin(); itFirst != First[*itList].end(); itFirst++)
				{
					if (itFirst->content == "EPS")
					{
						isEps = true;
					}
					else // ����else��Ϊ�˲�����eps
					{
						if (retSet.insert(*itFirst).second == true)
							isAddFirst = true;
					}
				}

				// ���û��eps����ô�Ͳ���Ҫ���������Ҳ����Ŵ�
				if (isEps == false)
					break;
			}
		}// end of ����չ����
		// �������������Ŀ�ܹ����������һ�����ţ���˵����Ҫ����eps��Ϊ���󲿷��ŵ�First����
		if (itList == S.end())
		{
			if (retSet.insert(Symbol("EPS", true)).second == true)
				isAddFirst = true;
		}

		// ���First�����䣬������while
		if (!isAddFirst)
			break;
	}// end of while

	return retSet;
}

// ��һ��״̬����һ��״̬���ϣ���ʵ����հ��Ĺ��̣�
I ParAnalyst::Closure(Item item)
{
	I ret;

	// ����ǹ�Լ��Ŀ
	// �����������ս��
	if (item.posPoint == Productions[item.idProduction].rcontent.size() || isVt(Productions[item.idProduction].rcontent[item.posPoint].content))
	{
		ret.Items.insert(item);
	}
	// ������Ƿ��ս��
	else
	{
		ret.Items.insert(item);
		vector<Production>::iterator itProduction;
		for (itProduction = Productions.begin(); itProduction != Productions.end(); itProduction++)
		{
			if (itProduction->lcontent == Productions[item.idProduction].rcontent[item.posPoint])
			{
				// ������Ҫ���ҵ��Ĳ���ʽ������I������������Ҫ��һ��չ����
				list<Symbol> SymList; //���� a

				// ����size��������
				// ���ұߵĲ���ʽ���η���SymList����Ϊ���չ��������ڲ���
				for (int cnt = 1; item.posPoint + cnt < (int)(Productions[item.idProduction].rcontent.size()); cnt++)
				{
					SymList.push_back(Productions[item.idProduction].rcontent[item.posPoint + cnt]);
				}
				SymList.push_back(item.expect); //�ǵ÷�a

				set<Symbol> retSet; //��õ�չ����
				retSet = getExpect(SymList);

				// ����Щչ�������μ���I��ret����
				set<Symbol>::iterator itSymbol;
				for (itSymbol = retSet.begin(); itSymbol != retSet.end(); itSymbol++)
				{
					I temp = Closure(Item(0, *itSymbol, itProduction->id));

					set<Item>::iterator itItem;
					for (itItem = temp.Items.begin(); itItem != temp.Items.end(); itItem++)
					{
						if (ret.Items.insert(*itItem).second == 0)
						{
							//cout << "�ظ�����ͬ����Ԫ��" << endl;
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
	cout << "�����ķ���LR(1)���У����Ժ����ﲻ�ǿ�����..." << endl;

	// �������½���I0״̬����I0״̬������ȥ�����״̬
	I itemp = Closure(Item(0, Symbol("#", true), 0));
	DFA.status.push_back(itemp);

	int idSNow = 0;
	// ����ÿһ��״̬��ÿһ����Ŀ
	list<I>::iterator itStatus;
	for (itStatus = DFA.status.begin(); itStatus != DFA.status.end(); itStatus++, idSNow++)
	{
		set<Item>::iterator itItem;
		int cnt = 0;
		for (itItem = itStatus->Items.begin(), cnt = 0; itItem != itStatus->Items.end(); itItem++, cnt++)
		{
			// ��Ϊ��Լ��Ŀ���������
			if (Productions[itItem->idProduction].rcontent.size() == itItem->posPoint)
			{
				GOTO tempGOTO(idSNow, itItem->expect);
				if (itItem->idProduction == 0) // �õ�һ���ع��ķ���Լ
				{
					ActionTb[tempGOTO] = Behavior(accept, itItem->idProduction);
				}
				else
				{
					ActionTb[tempGOTO] = Behavior(reduct, itItem->idProduction);
				}
			}
			// ��Ϊ�ƽ���Ŀ ���� �����˷��ս����
			else
			{
				// ȡ�������ķ���
				Symbol nextSymbol = Productions[itItem->idProduction].rcontent[itItem->posPoint];

				// ���ж��Ƿ��Ѿ���֮ǰ����Ŀ�ҹ��˶�Ӧ��nextSymbol��״̬������о�ֱ��ȥ�ұ����Ŀ
				if (DFA.gotos.count(GOTO(idSNow, nextSymbol)) == 1)
					continue;

				// ����Ͳ�����״̬
				I StatusNew = Closure(Item(itItem->posPoint + 1, itItem->expect, itItem->idProduction));

				// ������Ҫ��һ���������Ŀ��nextSymbol�ǲ���Ҳ��һ���ģ�����ǣ����Ǹ���Ŀ�ıհ�ͬ��������״̬��������ͬһ���ƽ�
				if (itItem != itStatus->Items.end())
				{
					set<Item>::iterator nextItItem = itItem;
					nextItItem++;
					for (; nextItItem != itStatus->Items.end(); nextItItem++)
					{
						if (Productions[nextItItem->idProduction].rcontent.size() == nextItItem->posPoint)
							continue;
						// �����״̬֮�����ĿҲ���ƽ�nextSymbol
						else if (Productions[nextItItem->idProduction].rcontent[nextItItem->posPoint] == nextSymbol)
						{
							I StatusTemp = Closure(Item(nextItItem->posPoint + 1, nextItItem->expect, nextItItem->idProduction));
							StatusNew.Items.insert(StatusTemp.Items.begin(), StatusTemp.Items.end());
						}
					}
				}

				// ������Ҫ��һ���ǲ����Ѿ�����һ����״̬
				list<I>::iterator itHadS;
				int idS_ = 0;
				bool isFound = false;
				// ��������״̬�������ǲ����Ѿ�������һ����״̬
				for (itHadS = DFA.status.begin(); itHadS != DFA.status.end(); itHadS++, idS_++)
				{
					// ����Ѿ���������״̬�ˣ��ͽ���ǰ״̬����nextSymbolָ�����е�״̬
					if (itHadS->Items == StatusNew.Items)
					{
						DFA.gotos[GOTO(idSNow, nextSymbol)] = idS_;
						if (isVt(nextSymbol.content)) // �������ս��
							ActionTb[GOTO(idSNow, nextSymbol)] = Behavior(shift, idS_);
						else // �����Ƿ��ս�����������gototb
							GotoTb[GOTO(idSNow, nextSymbol)] = idS_;
						isFound = true;
						break;
					}
				}

				// �������״̬
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

		}// end of ������Ŀ
	}// end of ����״̬





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
	//cout << "����" << cntItem << "����Ŀ" << endl;
	//cout << "����" << cntStatus << "��״̬" << endl;



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
		cerr << "������ļ�outputAction.txtʧ��" << endl;
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
		cerr << "������ļ�outputGoto.txtʧ��" << endl;
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
	cout << "***********************ջ�ռ�" << cnt << "***********************" << endl;
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
		cerr << "������ļ�outputStack.txtʧ��" << endl;
		exit(-1);
	}

	out << "***********************ջ�ռ�" << cnt << "***********************" << endl;

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

		// �������ע�ͣ��ͼ���
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

		// ��ʼLR��������������ջ
		if (Bh.bh == shift) // ��Ϊ�ƽ�
		{
			SymbolStack.push(next);
			StatusStack.push(Bh.nextS);
			itToken++;
			continue;
		}

		else if (Bh.bh == reduct) // ��Ϊ��Լ
		{
			Production p = Productions[Bh.nextS];
			int popNum = p.rcontent.size();

			Symbol Symbolpushed = p.lcontent;

			// �����׶Σ�˳�����һ���м����
			if (Bh.nextS == 0) // P ::= N declare_list
			{
				Symbol _declare_list = pop();
				// Symbol _N = pop(); 
				// �����Ҿ����������ӵ�
				// �������������N���ڶ��庯�����ü�Ĺ�ϵ���ҵ���������
				// �����������ǲ���Ҫ�����Ӻ����Ĺ��̵��ã�����Ҿ����ƺ�����Ҫ�����N���޸Ĳ���ʽ
				// �����޸Ĳ���ʽΪP ::= declare_list
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
				// ����
			}
			else if (Bh.nextS == 9) // parameter ::= parameter_list
			{
				Symbol _parameter_list = pop();
				// ����
			}
			else if (Bh.nextS == 10) // parameter ::= 
			{
				// ����
			}
			else if (Bh.nextS == 11) // parameter_list :: = param
			{
				Symbol _param = pop();
				// ����
			}
			else if (Bh.nextS == 12) // parameter_list ::= param , parameter_list
			{
				Symbol _parameter_list = pop();
				Symbol _comma = pop();
				Symbol _param = pop(); 
				// ����
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
				// ����
			}
			else if (Bh.nextS == 48) // factor ::= ID     
			{
				Symbol _ID = pop();
				Symbolpushed.name = _ID.name;
			}
			else if (Bh.nextS == 49) // argument_list ::=     
			{
				// ����
			}
			else if (Bh.nextS == 50) // argument_list ::= expression    
			{
				Symbol _expression = pop();
				// ����
			}
			else if (Bh.nextS == 51) // argument_list ::= expression , argument_list    
			{
				Symbol _argument_list = pop();
				Symbol _comma = pop();
				Symbol _expression = pop();
				// ����
			}
			else
			{
				// ������Ӧջ��Ԫ��popNum��
				for (int cnt = 0; cnt < popNum; cnt++)
				{
					SymbolStack.pop();
					StatusStack.pop();
				}
				
			}

			// ѹ���Ӧ��Goto���е�״̬����Լʽ�����
			SymbolStack.push(Symbolpushed);
			StatusStack.push(GotoTb[GOTO(StatusStack.top(), p.lcontent)]);
		}

		else if (Bh.bh == accept) // ����
		{
			cout << "accept!" << endl;
			cout << "�ɹ���Լ��Ŀ�������������" << endl;
			code.outputCode();
			code.outputCodeToFile();
			break;
		}

		else // error
		{
			cout << "��Լ���̳��ִ�������������������������ջ�ϵ�ֵ�Ѿ�������outputStack.txt��" << endl;
			break;
		}

	}

	return OK;
}

