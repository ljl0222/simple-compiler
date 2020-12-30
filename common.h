#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <stack>
#include <cstdlib>

using namespace std;

// 一些定义
const int MAXLINECNT = 1024;
const int MAXCOMMENTCNT = 2048;

// 函数返回状态
typedef int Status;
const int ERROR = -1;
const int OK = 0;

const int ENDINPUT = -2;

// 词法分析种类
typedef enum
{
	// 关键字
	// int void if else while return
	INT, VOID, IF, ELSE, WHILE, RETURN,

	// 标识符(变量名)
	ID,

	// 数字
	NUM,

	// 赋值
	// =
	ASSIGN,

	// 算符
	// +  -      *    /    ==      >   >=   <   <=   !=
	PLUS, MINUS, MUL, DIV, EQUAL, GT, GTE, LT, LTE, NE,

	// 界符
	// ;
	SEMICOLON,

	// 分隔符
	// ,
	COMMA,

	// 注释符
	// /*      */         //        /* */
	LPCOMMENT, RPCOMMENT, LCOMMENT, PCOMMENT,

	// 括号
	// (      )
	LBRACKET, RBRACKET,

	// 花括号
	// {    }
	LBRACE, RBRACE,

	// 换行
	// \n
	NEXTL,

	// 结束符
	// eof
	ENDFILE
} LexType;

// 二元组定义
typedef pair<LexType, string> Token;

// 一些简单的函数定义
bool isNum(char c);
bool isAlpha(char c);
bool isVt(string s);
string transTokenFirst(Token t);
list<int> merge(list<int> l1, list<int> l2);