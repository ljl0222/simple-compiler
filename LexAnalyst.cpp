#include "LexAnalyst.h"

// 打开文件
Status LexAnalyst::openInputFile(string path)
{
	input.open(path, ios::in);
	if (!input.is_open())
	{
		cerr << "打开输入文件" << path << "失败" << endl;
		exit(ERROR);
	}
	return OK;
}

// 初始化分析器，打开对应的输入文件
LexAnalyst::LexAnalyst()
{
	openInputFile("LexInput.txt");
}

LexAnalyst::LexAnalyst(const char* path)
{
	openInputFile(path);
}

// 拿到一个字符
char LexAnalyst::getChar()
{
	char tempChar;
	while (input >> tempChar)
	{
		if (tempChar == ' ' || tempChar == '\t')
			continue;
		break;
	}
	if (input.eof())
		return ENDINPUT;

	return tempChar;
}

// 拿到一个二元组
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
		// 单行注释，就像我一样QAQ
		if (input.peek() == '/')
		{
			char tempString[MAXLINECNT];
			tempString[0] = '/';
			input.getline(tempString + 1, MAXLINECNT, '\n');
			return Token(LCOMMENT, tempString);
		}
		/* 多行注释，就像我一样QAQ */
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
			// 正常情况下不会执行到这里，执行到这里说明匹配不到*/
			cerr << "匹配不到*/" << endl;
			exit(ERROR);
		}
		// 除法
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
			cerr << "这是一个取否符号，不符合我们的简单类C语法规则" << endl;
			exit(ERROR);
		}
	}
	else if (c == '#')
		return Token(ENDFILE, "#");
	else if (c == '\n') // 然后发现这里好像不行，上面采用流输入的时候无法读出换行
		return Token(NEXTL, "\\n");
	// 进入到这里，说明是字母或数字或乱码（乱码就错了，else判断掉）
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
				// 说明已经读到了不是数字的东西了
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
				// 说明已经读到了这个标识符/关键字以外的东西了
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
			cerr << "无法判断的符号" << endl;
			exit(ERROR);
		}
	}

	// 整个大判断的外面，但是这显然是不可能的，随便写一下
	cerr << "未知错误" << endl;
	exit(ERROR);
}



// 分析过程
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

// 输出到控制台
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

// 输出到文件
Status LexAnalyst::OutputToFile()
{
	ofstream out;
	out.open("outputLex.txt", ios::out);
	if (!out.is_open())
	{
		cerr << "打开输出文件outputLex.txt失败" << endl;
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