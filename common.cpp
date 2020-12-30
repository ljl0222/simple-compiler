#include "common.h"

bool isNum(char c)
{
	return (c >= '0' && c <= '9') ? true : false;
}

bool isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') ? true : false;
}

bool isVt(string s)
{
	return s == "int" || s == "void" || s == "if" || s == "else" || s == "while" || s == "return" || s == "ID" || s == "num"
		|| s == ">" || s == "<" || s == ">=" || s == "<=" || s == "==" || s == "!="
		|| s == "," || s == ";" || s == "+" || s == "-" || s == "*" || s == "/" || s == "="
		|| s == "(" || s == ")" || s == "{" || s == "}" || s == "#";
	// || s == "a" || s == "b";
}

// ת��Token�е�First��������
string transTokenFirst(Token t)
{
	const char words[31][10] = {
		// �ؼ���
		// int void if else while return
		"INT", "VOID", "IF", "ELSE", "WHILE", "RETURN",

		// ��ʶ��(������)
		"ID",

		// ����
		"NUM",

		// ��ֵ
		// =
		"ASSIGN",

		// ���
		// +  -      *    /    ==      >   >=   <   <=   !=
		"PLUS", "MINUS", "MUL", "DIV", "EQUAL", "GT", "GTE", "LT", "LTE", "NE",

		// ���
		// ;
		"SEMICOLON",

		// �ָ���
		// ,
		"COMMA",

		// ע�ͷ�
		// /*      */         //        /* */
		"LPCOMMENT", "RPCOMMENT", "LCOMMENT", "PCOMMENT",

		// ����
		// (      )
		"LBRACKET", "RBRACKET",

		// ������
		// {    }
		"LBRACE", "RBRACE",

		// ����
		// \n
		"NEXTL",

		// ������
		// eof
		"ENDFILE"
	};

	string str = words[t.first];

	return str;
}

list<int> merge(list<int> l1, list<int> l2)
{
	list<int> l;
	l.assign(l1.begin(), l1.end());
	l.splice(l.end(), l2);
	return l;
}