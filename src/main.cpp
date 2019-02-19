#include "engine.hpp"

int main(int argcc, char* argv[]) {
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	printf("#####Memengine Has Been Started!#####\n");
	Engine::getInstance()->run();
	printf("#####Memengine Has Been Terminated!####\n");
	//getchar();
	return 0;
}