#pragma once

#include "common.h"

class LexAnalyst
{
private:
	list<Token> result;
	ifstream input;
	Status openInputFile(string path);
	char getChar();
	Token getToken();

public:
	LexAnalyst();
	LexAnalyst(const char* path);
	// ~LexAnalyst();
	Status Analyse();
	Status Output();
	list<Token>& getRes();
	Status OutputToFile();
};