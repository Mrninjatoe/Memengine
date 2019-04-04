#include "engine.hpp"
#include <time.h>

int main(int argcc, char* argv[]) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtDumpMemoryLeaks();

	srand(1);
	printf("#####Memengine Has Been Started!#####\n");
	Engine::getInstance()->run();
	printf("#####Memengine Has Been Terminated!####\n");
	return 0;
}