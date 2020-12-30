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

// 转换Token中的First项，便于输出
string transTokenFirst(Token t)
{
	const char words[31][10] = {
		// 关键字
		// int void if else while return
		"INT", "VOID", "IF", "ELSE", "WHILE", "RETURN",

		// 标识符(变量名)
		"ID",

		// 数字
		"NUM",

		// 赋值
		// =
		"ASSIGN",

		// 算符
		// +  -      *    /    ==      >   >=   <   <=   !=
		"PLUS", "MINUS", "MUL", "DIV", "EQUAL", "GT", "GTE", "LT", "LTE", "NE",

		// 界符
		// ;
		"SEMICOLON",

		// 分隔符
		// ,
		"COMMA",

		// 注释符
		// /*      */         //        /* */
		"LPCOMMENT", "RPCOMMENT", "LCOMMENT", "PCOMMENT",

		// 括号
		// (      )
		"LBRACKET", "RBRACKET",

		// 花括号
		// {    }
		"LBRACE", "RBRACE",

		// 换行
		// \n
		"NEXTL",

		// 结束符
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