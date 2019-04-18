#include <stdlib.h>
#include <crtdbg.h>
#include <time.h>
#include "engine.hpp"

int main(int argcc, char* argv[]) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand(1);
	printf("#####Memengine Has Been Started!#####\n");
	Engine::getInstance()->run();
	printf("#####Memengine Has Been Terminated!####\n");

	_CrtDumpMemoryLeaks();
	//getchar();
	return 0;
}