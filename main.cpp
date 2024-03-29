#include "common.h"
#include "LexAnalyst.h"
#include "ParAnalyst.h"
#include "ObjectAnalyst.h"

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

	IntermediateCode* iCode = PA.getIntermediateCode();
	iCode->divideBaseBlocks(PA.getFunctionEnter());
	iCode->outputBlocks();

	ObjectAnalyst OA;
	OA.analyse(iCode->getFunctionBlocks());
	OA.outputBlocksAttachInfo();
	OA.productObjectCode_total();
	OA.outputObjectCode();
	return 0;
}