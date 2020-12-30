#include "common.h"
#include "LexAnalyst.h"
#include "ParAnalyst.h"

int main()
{
	LexAnalyst LA("inputLex.txt");
	LA.Analyse();
	// LA.Output();
	LA.OutputToFile();

	ParAnalyst PA("inputProductions-test.txt");
	PA.getProductions();
	// PA.showProductions();
	PA.initFirst();
	// PA.showFirst();
	PA.createDFA();
	// PA.outputAction();
	// PA.outputGoto();
	PA.outputActionToFile();
	PA.outputGotoToFile();
	PA.LRAnalyse(LA.getRes());

	return 0;
}