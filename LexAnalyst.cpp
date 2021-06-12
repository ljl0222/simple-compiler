#include "LexAnalyst.h"

// ���ļ�
Status LexAnalyst::openInputFile(string path)
{
	input.open(path, ios::in);
	if (!input.is_open())
	{
		cerr << "�������ļ�" << path << "ʧ��" << endl;
		exit(ERROR);
	}
	return OK;
}

// ��ʼ�����������򿪶�Ӧ�������ļ�
LexAnalyst::LexAnalyst()
{
	openInputFile("LexInput.txt");
}

LexAnalyst::LexAnalyst(const char* path)
{
	openInputFile(path);
}

// �õ�һ���ַ�
char LexAnalyst::getChar()
{
	char tempChar;
	while (tempChar = input.get())
	{
		if (tempChar == ' ' || tempChar == '\t')
			continue;
		break;
	}
	if (input.eof())
		return ENDINPUT;

	return tempChar;
}

// �õ�һ����Ԫ��
Token LexAnalyst::getToken()
{
	char c = getChar();
	if (c == '(')
		return Token(LBRACKET, "(");
	else if (c == ')')
		return Token(RBRACKET, ")");
	else if (c == '{')
		return Token(LBRACE, "{");
	else if (c == '}')
		return Token(RBRACE, "}");
	else if (c == '+')
		return Token(PLUS, "+");
	else if (c == '-')
		return Token(MINUS, "-");
	else if (c == '*')
		return Token(MUL, "*");
	else if (c == ';')
		return Token(SEMICOLON, ";");
	else if (c == ',')
		return Token(COMMA, ",");
	else if (c == '/')
	{
		// ����ע�ͣ�������һ��QAQ
		if (input.peek() == '/')
		{
			char tempString[MAXLINECNT];
			tempString[0] = '/';
			input.getline(tempString + 1, MAXLINECNT, '\n');
			return Token(LCOMMENT, tempString);
		}
		/* ����ע�ͣ�������һ��QAQ */
		else if (input.peek() == '*')
		{
			input.get();
			string tempString = "/*";
			while (input >> c)
			{
				tempString += c;
				if (c == '*')
				{
					if (input.peek() == '/')
					{
						input >> c;
						tempString += c;
						return Token(PCOMMENT, tempString);
					}
					else
						continue;
				}
			}
			// ��������²���ִ�е����ִ�е�����˵��ƥ�䲻��*/
			cerr << "ƥ�䲻��*/" << endl;
			exit(ERROR);
		}
		// ����
		else
		{
			return Token(DIV, "/");
		}
	}
	else if (c == '=')
	{
		if (input.peek() == '=')
		{
			input.get();
			return Token(EQUAL, "==");
		}
		else
			return Token(ASSIGN, "=");
	}
	else if (c == '>')
	{
		if (input.peek() == '=')
		{
			input.get();
			return Token(GTE, ">=");
		}
		else
			return Token(GT, ">");
	}
	else if (c == '<')
	{
		if (input.peek() == '=')
		{
			input.get();
			return Token(LTE, "<=");
		}
		else
			return Token(LT, "<");
	}
	else if (c == '!')
	{
		if (input.peek() == '=')
		{
			input.get();
			return Token(NE, "!=");
		}
		else
		{
			cerr << "����һ��ȡ����ţ����������ǵļ���C�﷨����" << endl;
			exit(ERROR);
		}
	}
	else if (c == '#')
		return Token(ENDFILE, "#");
	else if (c == '\n') 
		return Token(NEXTL, "\\n");
	// ���뵽���˵������ĸ�����ֻ����루����ʹ��ˣ�else�жϵ���
	else
	{
		if (isNum(c))
		{
			string tempString;
			tempString += c;
			while (1)
			{
				char tempNum = input.peek();
				if (isNum(tempNum))
				{
					input.get();
					tempString += tempNum;
				}
				// ˵���Ѿ������˲������ֵĶ�����
				else
					break;
			}
			return Token(NUM, tempString);
		}
		else if (isAlpha(c))
		{
			string tempString;
			tempString += c;
			while (1)
			{
				char tempCh = input.peek();
				if (isAlpha(tempCh) || isNum(tempCh))
				{
					input.get();
					tempString += tempCh;
				}
				// ˵���Ѿ������������ʶ��/�ؼ�������Ķ�����
				else
					break;
			}
			if (tempString == "int")
				return Token(INT, tempString);
			else if (tempString == "void")
				return Token(VOID, tempString);
			else if (tempString == "if")
				return Token(IF, tempString);
			else if (tempString == "else")
				return Token(ELSE, tempString);
			else if (tempString == "while")
				return Token(WHILE, tempString);
			else if (tempString == "return")
				return Token(RETURN, tempString);
			else
				return Token(ID, tempString);
		}
		else
		{
			cerr << "�޷��жϵķ���"  << c << endl;
			exit(ERROR);
		}
	}

	// �������жϵ����棬��������Ȼ�ǲ����ܵģ����дһ��
	cerr << "δ֪����" << endl;
	exit(ERROR);
}



// ��������
Status LexAnalyst::Analyse()
{
	while (1)
	{
		Token tempToken = getToken();
		result.push_back(tempToken);
		if (tempToken.first == ENDFILE)
			break;
	}
	return OK;
}

// ���������̨
Status LexAnalyst::Output()
{
	list <Token>::iterator resultIterator;
	for (resultIterator = result.begin(); resultIterator != result.end(); resultIterator++)
	{
		cout << '<' << transTokenFirst(*resultIterator) << ',' << resultIterator->second << '>' << endl;
	}
	return OK;
}

list<Token>& LexAnalyst::getRes()
{
	return result;
}

// ������ļ�
Status LexAnalyst::OutputToFile()
{
	ofstream out;
	out.open("outputLex.txt", ios::out);
	if (!out.is_open())
	{
		cerr << "������ļ�outputLex.txtʧ��" << endl;
		exit(-1);
	}
	list <Token>::iterator resultIterator;
	for (resultIterator = result.begin(); resultIterator != result.end(); resultIterator++)
	{
		out << '<' << transTokenFirst(*resultIterator) << ',' << resultIterator->second << '>' << endl;
	}

	out.close();
	return OK;
}